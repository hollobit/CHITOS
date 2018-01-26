package kr.re.etri.smartwearableclient;

import android.os.Bundle;
import android.os.Handler;
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
import org.json.JSONException;
import org.json.JSONObject;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.EnumSet;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class SmartwatchFragment extends Fragment implements OcPlatform.OnResourceFoundListener, OcResource.OnGetListener {
    ArrayList<String> resourceURIfilter = new ArrayList<>();

    View view;
    JSONObject data;

    TextView tv_date;
    TextView tv_time;
    TextView tv_value_heartrate;
    TextView tv_unit_heartrate;
    Button btn_read;

    Handler handler = new Handler();
    Runnable runnableEnableReadBtn = new Runnable() {
        @Override
        public void run() {
            btn_read.setEnabled(true);
        }
    };

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        view = inflater.inflate(R.layout.fragment_smartwatch, container, false);

        resourceURIfilter.add("/ClockResURI");
        resourceURIfilter.add("/HeartrateResURI");

        tv_date = view.findViewById(R.id.tv_date);
        tv_time = view.findViewById(R.id.tv_time);
        tv_value_heartrate = view.findViewById(R.id.tv_value_heartrate);
        tv_unit_heartrate = view.findViewById(R.id.tv_unit_heartrate);

        btn_read = view.findViewById(R.id.btn_read);
        btn_read.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                view.setEnabled(false);

                data = new JSONObject();
                findResource();

                handler.postDelayed(runnableEnableReadBtn, 3000);
            }
        });
        return view;
    }

    @Override
    public synchronized void onResourceFound(OcResource ocResource) {
        if(ocResource != null) {
            if(resourceURIfilter.contains(ocResource.getUri())) {
                Log.append("Founded resource: " + ocResource.getUri());

                Map<String, String> queryParams = new HashMap<>();
                try {
                    ocResource.get(queryParams, this);
                } catch (OcException e) {
                    Log.append("Error occurred while invoking \"get\" API");
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
        try {
            ResponseParser parser = new ResponseParser();

            switch(ocRepresentation.getUri()) {
                case "/ClockResURI":
                    data.put("clock", parser.parseClockResURI(ocRepresentation));
                    break;
                case "/HeartrateResURI":
                    data.put("heartrate", parser.parseHeartrateResURI(ocRepresentation));
                    break;
                default:
                    break;
            }

            if(!data.isNull("clock") && !data.isNull("heartrate")) {
                setClockValue(((JSONObject)data.get("clock")).get("datetime").toString());
                setHeartrateValue(((JSONObject)data.get("heartrate")).get("heartrate").toString(), "bpm");
            }

            Log.append(data.toString());
        } catch(JSONException e) {
            Log.append("JSONExcpetion @SmartwatchFragment\n" + e.getMessage());
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
            OcPlatform.findResource("",
                    OcPlatform.WELL_KNOWN_QUERY,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT),
                    this
            );

        } catch (OcException e) {
            Log.append("Failed to invoke find resource API");
        }
    }

    private void setClockValue(final String iso8601String) {
        try {
            final Date dt = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ssZ").parse(iso8601String);

            getActivity().runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    tv_time.setText(new SimpleDateFormat("HH:mm:ss").format(dt));
                    tv_date.setText(new SimpleDateFormat("yyyy-MM-dd").format(dt));
                }
            });
        } catch(ParseException e) {
            Log.append("Error: ParseException @SmartwatchFragment\n" + e.getMessage());
        }
    }

    private void setHeartrateValue(final String value, final String unit) {
        getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                tv_value_heartrate.setText(value);
                tv_unit_heartrate.setText(unit);

                handler.removeCallbacks(runnableEnableReadBtn);
                btn_read.setEnabled(true);
            }
        });
    }
}
