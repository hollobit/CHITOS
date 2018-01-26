package kr.re.etri.smartwearableclient;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.ToggleButton;

import org.iotivity.base.ErrorCode;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.EnumSet;
import java.util.HashMap;
import java.util.List;

public class ScaleFragment extends Fragment implements OcPlatform.OnDeviceFoundListener, OcPlatform.OnResourceFoundListener, OcResource.OnGetListener, OcResource.OnPostListener {
    private View view;
    private final String DEVICE_TYPE = "oic.d.bodyscale";
    private List<ResourceItem> resFilter = new ArrayList<>();
    private ResponseParser parser = new ResponseParser();
    private HashMap<String, String> p;

    TextView tv_weight;
    TextView tv_unit;
    ToggleButton tb_gender;
    EditText et_height;
    EditText et_age;
    TextView tv_bmi;
    Button btn_read;

    int post_cnt;
    char sex;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        view = inflater.inflate(R.layout.fragment_scale, container, false);

        tv_weight = view.findViewById(R.id.tv_weight);
        tv_unit = view.findViewById(R.id.tv_unit);
        tb_gender = view.findViewById(R.id.tb_gender);
        et_height = view.findViewById(R.id.et_height);
        et_age = view.findViewById(R.id.et_age);
        tv_bmi = view.findViewById(R.id.tv_bmi);
        btn_read = view.findViewById(R.id.btn_read);

        tb_gender.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if(isChecked) {
                    sex = 'M';
                } else {
                    sex = 'F';
                }
            }
        });
        tb_gender.setChecked(true);
        sex = 'M';

        post_cnt = 0;

        btn_read.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(final View view) {
                tv_weight.setText("--");
                if(et_height.getText().length() == 0) { et_height.setText("1.8"); }
                if(et_age.getText().length() == 0) { et_age.setText("39"); }
                tv_bmi.setText("--");

                resFilter.clear();

                p = new HashMap<>(); p.put("if", "oic.if.b");
                resFilter.add(new ResourceItem(DEVICE_TYPE,"oic.r.height", "/HeightResURI", null, "POST"));
                resFilter.add(new ResourceItem(DEVICE_TYPE,"kr.re.etri.r.userinfo", "/UserInfoResURI", null, "POST"));

                findResource();
            }
        });

        return view;
    }

    @Override
    public void onDeviceFound(OcRepresentation ocRepresentation) {
        if(ocRepresentation.getResourceTypes().contains(DEVICE_TYPE)) {
            try {
                OcPlatform.findResource(ocRepresentation.getHost(),
                        OcPlatform.WELL_KNOWN_QUERY,
                        EnumSet.of(OcConnectivityType.CT_DEFAULT),
                        this
                );
            } catch (OcException e) {
                Log.append("Failed to invoke find resource API");
            }
        }
    }

    @Override
    public synchronized void onResourceFound(OcResource ocResource) {
        for(ResourceItem item: resFilter) {
            String uri = item.getUri();
            if(uri.indexOf('?') > -1) { uri = uri.substring(0, uri.indexOf('?')); }

            if(ocResource.getUri().equals(uri)) {
                Log.append("Founded resource: " + ocResource.getUri());
                if(ocResource.getAllHosts().size() > 0) {
                    Log.append("access : " + ocResource.getAllHosts().get(0));
                    ocResource.setHost(ocResource.getAllHosts().get(0));
                } else {
                    Log.append("access : " + ocResource.getHost());
                }

                if(item.getMethod().compareTo("GET") == 0) {
                    Log.append("access GET");
                    try {
                        ocResource.get(item.getParams(), this);
                    } catch (OcException e) {
                        Log.append("Error occurred while invoking \"get\" API");
                    }
                } else if(item.getMethod().compareTo("POST") == 0) {
                    Log.append("access POST");
                    OcRepresentation rep = new OcRepresentation();

                    try {
                        switch(item.getUri()) {
                            case "/HeightResURI":
                                rep.setValue("id", "unique_example_id");
                                rep.setValue("height", Double.parseDouble(et_height.getText().toString()));
                                rep.setValue("unit", "m");
                                break;
                            case "/UserInfoResURI":
                                rep.setValue("gender", ((sex == 'M') ? 1 : 0));
                                rep.setValue("age", Integer.parseInt(et_age.getText().toString()));
                                break;
                            default:
                                break;
                        }
                    } catch (OcException e) {
                        Log.append("Failed to make OcRepresentation for POST request");
                    }

                    try {
                        ocResource.post(rep, item.getParams(), this);
                    } catch (OcException e) {
                        Log.append("Error occurred while invoking \"post\" API");
                    }
                } else {
                    Log.append("access ...");
                }
            }
        }
    }

    @Override
    public synchronized void onFindResourceFailed(Throwable throwable, String s) {
        Log.append("Failed: find resource @ " + getClass().getSimpleName() + "\n" + throwable.getMessage());
    }

    @Override
    public synchronized void onGetCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation) {
        HashMap<String, String> data;

        switch(ocRepresentation.getUri()) {
            case "/BodyScaleAMResURI":
                data = getWeightBMI(parser.parseBloodPressureMonitorAMResURI_B(ocRepresentation));
                final String weight = data.get("weight");
                final String unit = data.get("unit");
                final String bmi = data.get("bmi");

                getActivity().runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        tv_weight.setText(weight);
                        tv_unit.setText(unit);
                        tv_bmi.setText(bmi);
                    }
                });
            default:
                break;
        }
    }

    @Override
    public synchronized void onGetFailed(Throwable throwable) {
        if (throwable instanceof OcException) {
            OcException ocEx = (OcException) throwable;
            ErrorCode errCode = ocEx.getErrorCode();

            Log.append("Error: GET representation (errCode: " + errCode + ")\n" + ocEx.getMessage());
        } else {
            Log.append("Error: GET representation");
        }
    }

    @Override
    public void onPostCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation) {
        post_cnt++;

        if(post_cnt >= 2) {
            post_cnt = 0;
            resFilter.clear();
            resFilter.add(new ResourceItem(DEVICE_TYPE,"oic.r.bodyscale-am", "/BodyScaleAMResURI?if=oic.if.b", p));
            findResource();
        }
    }

    @Override
    public void onPostFailed(Throwable throwable) {
        if (throwable instanceof OcException) {
            OcException ocEx = (OcException) throwable;
            ErrorCode errCode = ocEx.getErrorCode();

            Log.append("Error: POST representation (errCode: " + errCode + ")\n" + ocEx.getMessage());
        } else {
            Log.append("Error: POST representation");
        }
    }

    private void findResource() {
        try {
            OcPlatform.getDeviceInfo("",
                    OcPlatform.WELL_KNOWN_DEVICE_QUERY,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT),
                    this
            );
        } catch (OcException e) {
            Log.append("Failed to invoke find device API");
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////

    private HashMap<String, String> getWeightBMI(JSONArray arr) {
        HashMap<String, String> ret = new HashMap<>();

        try {
            for(int i = 0; i < arr.length(); i++) {
                JSONObject obj = (JSONObject) arr.get(i);
                if(obj.get("href").equals("/myWeightResURI")) {

                    ret.put("weight", String.valueOf(Math.floor((double) ((JSONObject) obj.get("rep")).get("weight") * 10) / 10));
                    ret.put("unit", ((JSONObject) obj.get("rep")).get("unit").toString());
                } else if(obj.get("href").equals("/myBMIResURI")) {
                    ret.put("bmi", ((JSONObject) obj.get("rep")).get("bmi").toString());
                }
            }
        } catch(JSONException e) {
            Log.append("JSONExcpetion @ScaleFragment\n" + e.getMessage());
        }

        return ret;
    }
}
