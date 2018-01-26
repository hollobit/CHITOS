package kr.re.etri.smartwearableclient;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

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

public class BpmFragment extends Fragment implements OcPlatform.OnDeviceFoundListener, OcPlatform.OnResourceFoundListener, OcResource.OnGetListener {
    private View view;
    private final String DEVICE_TYPE = "oic.d.bloodpressuremonitor";
    private List<ResourceItem> resFilter = new ArrayList<>();
    private ResponseParser parser = new ResponseParser();
    private HashMap<String, String> p;

    TextView tv_value_systolic;
    TextView tv_unit_systolic;
    TextView tv_value_diastolic;
    TextView tv_unit_diastolic;
    TextView tv_value_heartrate;
    Button btn_start;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        view = inflater.inflate(R.layout.fragment_bpm, container, false);

        tv_value_systolic = view.findViewById(R.id.tv_value_systolic);
        tv_unit_systolic = view.findViewById(R.id.tv_unit_systolic);
        tv_value_diastolic = view.findViewById(R.id.tv_value_diastolic);
        tv_unit_diastolic = view.findViewById(R.id.tv_unit_diastolic);
        tv_value_heartrate = view.findViewById(R.id.tv_value_heartrate);
        btn_start = view.findViewById(R.id.btn_start);

        btn_start.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                tv_value_systolic.setText("-");
                tv_unit_systolic.setText("mmHg");
                tv_value_diastolic.setText("-");
                tv_unit_diastolic.setText("mmHg");
                tv_value_heartrate.setText("-");

                findResource();
            }
        });

        p = new HashMap<>(); p.put("if", "oic.if.b");
        resFilter.add(new ResourceItem(DEVICE_TYPE,"oic.r.bloodpressuremonitor-am", "/BloodPressureMonitorAMResURI?if=oic.if.b", p));

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
            case "/BloodPressureMonitorAMResURI":
                data = getBloodPressure(parser.parseBloodPressureMonitorAMResURI_B(ocRepresentation));
                final String systolic_value = data.get("systolic");
                final String diastolic_value = data.get("diastolic");
                final String unit = data.get("unit");
                final String pulserate = data.get("pulserate");

                getActivity().runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        tv_value_systolic.setText(systolic_value);
                        tv_value_diastolic.setText(diastolic_value);
                        tv_unit_systolic.setText(unit);
                        tv_unit_diastolic.setText(unit);
                        tv_value_heartrate.setText(pulserate);
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

    private HashMap<String, String> getBloodPressure(JSONArray arr) {
        HashMap<String, String> ret = new HashMap<>();

        try {
            for(int i = 0; i < arr.length(); i++) {
                JSONObject obj = (JSONObject) arr.get(i);
                if(obj.get("href").equals("/myBloodPressureResURI")) {
                    ret.put("systolic", String.valueOf(((JSONObject) obj.get("rep")).get("systolic")));
                    ret.put("diastolic", String.valueOf(((JSONObject) obj.get("rep")).get("diastolic")));
                    ret.put("unit", ((JSONObject) obj.get("rep")).get("unit").toString());
                } else if(obj.get("href").equals("/myPulseRateResURI")) {
                    ret.put("pulserate", ((JSONObject) obj.get("rep")).get("pulserate").toString());
                }
            }
        } catch(JSONException e) {
            Log.append("JSONExcpetion @BpmFragment\n" + e.getMessage());
        }

        return ret;
    }
}
