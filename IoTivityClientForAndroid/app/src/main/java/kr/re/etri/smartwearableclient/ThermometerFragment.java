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

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.EnumSet;
import java.util.HashMap;
import java.util.List;

public class ThermometerFragment extends Fragment implements OcPlatform.OnDeviceFoundListener, OcPlatform.OnResourceFoundListener, OcResource.OnGetListener {
    private View view;
    private final String DEVICE_TYPE = "oic.d.bodythermometer";
    private List<ResourceItem> resFilter = new ArrayList<>();
    private ResponseParser parser = new ResponseParser();
    private HashMap<String, String> p;

    private TextView tv_value;
    private TextView tv_measure_at;
    private Button btn_read;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        view = inflater.inflate(R.layout.fragment_thermometer, container, false);

        tv_value = view.findViewById(R.id.tv_value);
        tv_measure_at = view.findViewById(R.id.tv_measured_at);
        btn_read = view.findViewById(R.id.btn_read);

        btn_read.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(final View view) {
                findResource();
            }
        });

        p = new HashMap<>(); p.put("if", "oic.if.b");
        resFilter.add(new ResourceItem(DEVICE_TYPE,"oic.r.body.bodythermometer", "/BodyThermometerAMResURI?if=oic.if.b", p));
        resFilter.add(new ResourceItem(DEVICE_TYPE,"oic.r.clock", "/ClockResURI", null));

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
            case "/BodyThermometerAMResURI":
                data = getTemperature(parser.parseBodyThermometerAMResURI_B(ocRepresentation));
                final String temperature = data.get("temperature");

                getActivity().runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        tv_value.setText(temperature);
                    }
                });
                break;
            case "/ClockResURI":
                data = getDatetime(parser.parseClockResURI(ocRepresentation));
                final String datetime = data.get("datetime");

                getActivity().runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        tv_measure_at.setText(datetime);
                    }
                });
                break;
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

    private HashMap<String, String> getTemperature(JSONArray arr) {
        HashMap<String, String> ret = new HashMap<>();

        try {
            for(int i = 0; i < arr.length(); i++) {
                JSONObject obj = (JSONObject) arr.get(i);
                if(obj.get("href").equals("/myTemperatureResURI")) {
                    ret.put("temperature", String.valueOf(Math.floor((double) ((JSONObject) obj.get("rep")).get("temperature") * 10) / 10));
                    ret.put("unit", ((JSONObject) obj.get("rep")).get("unit").toString());
                }
            }
        } catch(JSONException e) {
            Log.append("JSONExcpetion @ThermometerFragment\n" + e.getMessage());
        }

        return ret;
    }

    private HashMap<String, String> getDatetime(JSONObject obj) {
        HashMap<String, String> ret = new HashMap<>();

        try {
            String iso8601date = obj.getString("datetime");
            SimpleDateFormat sdf_in = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ssZ");
            SimpleDateFormat sdf_out = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
            Date dt = sdf_in.parse(iso8601date);

            ret.put("datetime", sdf_out.format(dt));

            return ret;
        } catch(JSONException e) {
            Log.append("JSONExcpetion @ThermometerFragment\n" + e.getMessage());
        } catch(ParseException e) {
            Log.append("ParseException @ThermometerFragment\n" + e.getMessage());
        }

        return ret;
    }
}
