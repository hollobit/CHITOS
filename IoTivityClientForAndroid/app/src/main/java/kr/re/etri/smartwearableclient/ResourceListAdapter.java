package kr.re.etri.smartwearableclient;

import android.content.Context;
import android.graphics.Color;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.TextView;

import org.iotivity.base.ErrorCode;
import org.iotivity.base.ObserveType;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.EnumSet;
import java.util.HashMap;
import java.util.List;

class ResourceListAdapter extends BaseAdapter implements
        OcPlatform.OnDeviceFoundListener,
        OcPlatform.OnResourceFoundListener,
        OcResource.OnGetListener,
        OcResource.OnPostListener,
        OcResource.OnObserveListener {
    private Context context;
    ResourceViewerFragment fragment;

    private List<ResourceItem> items = new ArrayList<>();
    private String obj_string;
    private ResourceItem selectedItem;

    ResourceListAdapter(Context context, ResourceViewerFragment fragment) {
        this.context = context;
        this.fragment = fragment;

        HashMap<String, String> p;

        ///////////////////////////////////////////////////////////////////////////////////////////
        // Thermometer
        ///////////////////////////////////////////////////////////////////////////////////////////
        items.add(new ResourceItem("oic.d.bodythermometer","oic.wk.d", "/oic/d", null));
        items.add(new ResourceItem("oic.d.bodythermometer","oic.wk.p", "/oic/p", null));

        p = new HashMap<>(); p.put("if", "oic.if.baseline");
        items.add(new ResourceItem("oic.d.bodythermometer","oic.r.bodythermometer-am", "/BodyThermometerAMResURI?if=oic.if.baseline", p));
        p = new HashMap<>(); p.put("if", "oic.if.ll");
        items.add(new ResourceItem("oic.d.bodythermometer","oic.r.body.bodythermometer", "/BodyThermometerAMResURI?if=oic.if.ll", p));
        p = new HashMap<>(); p.put("if", "oic.if.b");
        items.add(new ResourceItem("oic.d.bodythermometer","oic.r.body.bodythermometer", "/BodyThermometerAMResURI?if=oic.if.b", p));

        items.add(new ResourceItem("oic.d.bodythermometer","oic.r.body.location", "/BodyLocationResURI", null));
        items.add(new ResourceItem("oic.d.bodythermometer","oic.r.body.location.temperature", "/BodyLocationTemperatureResURI", null));
        items.add(new ResourceItem("oic.d.bodythermometer","oic.r.userid", "/UserIDResURI", null));
        items.add(new ResourceItem("oic.d.bodythermometer","oic.r.time.observed", "/ClockResURI", null));
        items.add(new ResourceItem("oic.d.bodythermometer","oic.r.time.observed", "/ClockResURI", null, "POST"));

        ///////////////////////////////////////////////////////////////////////////////////////////
        // Blood Pressure Monitor
        ///////////////////////////////////////////////////////////////////////////////////////////
        items.add(new ResourceItem("oic.d.bloodpressuremonitor","oic.wk.d", "/oic/d", null));
        items.add(new ResourceItem("oic.d.bloodpressuremonitor","oic.wk.p", "/oic/p", null));

        p = new HashMap<>(); p.put("if", "oic.if.b");
        items.add(new ResourceItem("oic.d.bloodpressuremonitor","oic.r.bloodpressuremonitor-am", "/BloodPressureMonitorAMResURI?if=oic.if.b", p));

        p = new HashMap<>(); p.put("if", "oic.if.ll");
        items.add(new ResourceItem("oic.d.bloodpressuremonitor","oic.r.bloodpressuremonitor-am", "/BloodPressureMonitorAMResURI?if=oic.if.ll", p));

        p = new HashMap<>(); p.put("if", "oic.if.baseline");
        items.add(new ResourceItem("oic.d.bloodpressuremonitor","oic.r.bloodpressuremonitor-am", "/BloodPressureMonitorAMResURI?if=oic.if.baseline", p));

        items.add(new ResourceItem("oic.d.bloodpressuremonitor","oic.r.blood.pressure", "/BloodPressureResURI", null));
        items.add(new ResourceItem("oic.d.bloodpressuremonitor","oic.r.pulserate", "/PulseRateResURI", null));
        items.add(new ResourceItem("oic.d.bloodpressuremonitor","oic.r.userid", "/UserIDResURI", null));
        items.add(new ResourceItem("oic.d.bloodpressuremonitor","oic.r.time.observed", "/ClockResURI", null));
        items.add(new ResourceItem("oic.d.bloodpressuremonitor","oic.r.time.observed", "/ClockResURI", null, "POST"));

        ///////////////////////////////////////////////////////////////////////////////////////////
        // Body Scale
        ///////////////////////////////////////////////////////////////////////////////////////////
        items.add(new ResourceItem("oic.d.bodyscale","oic.wk.d", "/oic/d", null));
        items.add(new ResourceItem("oic.d.bodyscale","oic.wk.p", "/oic/p", null));

        p = new HashMap<>(); p.put("if", "oic.if.b");
        items.add(new ResourceItem("oic.d.bodyscale","oic.r.bodyscale-am", "/BodyScaleAMResURI?if=oic.if.b", p));

        p = new HashMap<>(); p.put("if", "oic.if.ll");
        items.add(new ResourceItem("oic.d.bodyscale","oic.r.bodyscale-am", "/BodyScaleAMResURI?if=oic.if.ll", p));

        p = new HashMap<>(); p.put("if", "oic.if.baseline");
        items.add(new ResourceItem("oic.d.bodyscale","oic.r.bodyscale-am", "/BodyScaleAMResURI?if=oic.if.baseline", p));

        items.add(new ResourceItem("oic.d.bodyscale","oic.r.bmi", "/BMIResURI", null));
        items.add(new ResourceItem("oic.d.bodyscale","oic.r.body.fat", "/BodyFatResURI", null));
        items.add(new ResourceItem("oic.d.bodyscale","oic.r.body.ffm", "/BodyFatFreeMassResURI", null));
        items.add(new ResourceItem("oic.d.bodyscale","oic.r.body.slm", "/BodySoftLeanMassResURI", null));
        items.add(new ResourceItem("oic.d.bodyscale","oic.r.body.water", "/BodyWaterResURI", null));
        items.add(new ResourceItem("oic.d.bodyscale","oic.r.height", "/HeightResURI", null));
        items.add(new ResourceItem("oic.d.bodyscale","oic.r.height", "/HeightResURI", null, "POST"));
        items.add(new ResourceItem("oic.d.bodyscale","oic.r.weight", "/WeightResURI", null));
        items.add(new ResourceItem("oic.d.bodyscale","oic.r.userid", "/UserIDResURI", null));
        items.add(new ResourceItem("oic.d.bodyscale","oic.r.time.observed", "/ClockResURI", null));
        items.add(new ResourceItem("oic.d.bodyscale","oic.r.time.observed", "/ClockResURI", null, "POST"));
        items.add(new ResourceItem("oic.d.bodyscale","kr.re.etri.r.userinfo", "/UserInfoResURI", null, "POST"));

        ///////////////////////////////////////////////////////////////////////////////////////////
        // Glucose Monitor
        ///////////////////////////////////////////////////////////////////////////////////////////
        items.add(new ResourceItem("oic.d.glucosemeter","oic.wk.d", "/oic/d", null));
        items.add(new ResourceItem("oic.d.glucosemeter","oic.wk.p", "/oic/p", null));

        p = new HashMap<>(); p.put("if", "oic.if.b");
        items.add(new ResourceItem("oic.d.glucosemeter","oic.r.glucosemeter-am", "/GlucoseMeterAMResURI?if=oic.if.b", p));

        p = new HashMap<>(); p.put("if", "oic.if.ll");
        items.add(new ResourceItem("oic.d.glucosemeter","oic.r.glucosemeter-am", "/GlucoseMeterAMResURI?if=oic.if.ll", p));

        p = new HashMap<>(); p.put("if", "oic.if.baseline");
        items.add(new ResourceItem("oic.d.glucosemeter","oic.r.glucosemeter-am", "/GlucoseMeterAMResURI?if=oic.if.baseline", p));

        items.add(new ResourceItem("oic.d.glucosemeter","oic.r.duration", "/DurationResURI", null));
        items.add(new ResourceItem("oic.d.glucosemeter","oic.r.glucose", "/GlucoseResURI", null));
        items.add(new ResourceItem("oic.d.glucosemeter","oic.r.glucose.carb", "/GlucoseCarbResURI", null));
        items.add(new ResourceItem("oic.d.glucosemeter","oic.r.glucose.exercise", "/ExerciseResURI", null));
        items.add(new ResourceItem("oic.d.glucosemeter","oic.r.glucose.hba1c", "/GlucoseHbA1cResURI", null));
        items.add(new ResourceItem("oic.d.glucosemeter","oic.r.glucose.health", "/GlucoseHealthResURI", null));
        items.add(new ResourceItem("oic.d.glucosemeter","oic.r.glucose.meal", "/GlucoseMealResURI", null));
        items.add(new ResourceItem("oic.d.glucosemeter","oic.r.glucose.medication", "/GlucoseMedicationResURI", null));
        items.add(new ResourceItem("oic.d.glucosemeter","oic.r.glucose.samplelocation", "/GlucoseSampleLocationResURI", null));
        items.add(new ResourceItem("oic.d.glucosemeter","oic.r.glucose.tester", "/GlucoseTesterResURI", null));
        items.add(new ResourceItem("oic.d.glucosemeter","oic.r.userid", "/UserIDResURI", null));
        items.add(new ResourceItem("oic.d.glucosemeter","oic.r.time.observed", "/ClockResURI", null));
        items.add(new ResourceItem("oic.d.glucosemeter","oic.r.time.observed", "/ClockResURI", null, "POST"));

        ///////////////////////////////////////////////////////////////////////////////////////////
        // Smart Watch
        ///////////////////////////////////////////////////////////////////////////////////////////
        items.add(new ResourceItem("oic.d.smartwatch","oic.wk.d", "/oic/d", null));
        items.add(new ResourceItem("oic.d.smartwatch","oic.wk.p", "/oic/p", null));
        items.add(new ResourceItem("oic.d.smartwatch","oic.r.heartrate", "/HeartrateResURI", null));
        items.add(new ResourceItem("oic.d.smartwatch","oic.r.heartrate", "/HeartrateResURI", null, "OBSERVE"));
        items.add(new ResourceItem("oic.d.smartwatch","oic.r.heartrate", "/HeartrateResURI", null, "DEOBSERVE"));
        items.add(new ResourceItem("oic.d.smartwatch","oic.r.userid", "/UserIDResURI", null));
        items.add(new ResourceItem("oic.d.smartwatch","oic.r.clock", "/ClockResURI", null));

        notifyDataSetChanged();
    }

    @Override
    public View getView(int i, View view, ViewGroup viewGroup) {
        final ResourceItem item = getItem(i);

        if(view == null) {
            LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            view = inflater.inflate(R.layout.list_item_resource, viewGroup, false);
        }

        // background color
        if(item.getDeviceType().equals("oic.d.bloodpressuremonitor") || item.getDeviceType().equals("oic.d.glucosemeter")) {
            view.setBackgroundColor(Color.parseColor("#efefef"));
        } else {
            view.setBackgroundColor(Color.WHITE);
        }

        TextView tv_device = view.findViewById(R.id.tv_device);
        tv_device.setText(item.getDeviceType());

        TextView tv_type = view.findViewById(R.id.tv_type);
        tv_type.setText(item.getResourceType());

        TextView tv_method = view.findViewById(R.id.tv_method);
        tv_method.setText(item.getMethod());

        TextView tv_uri = view.findViewById(R.id.tv_uri);
        tv_uri.setText(item.getUri());

        Button btn_read = view.findViewById(R.id.btn_read);
        if(item.getMethod().compareTo("POST") == 0) {
            btn_read.setText("WRITE");
        } else {
            btn_read.setText("READ");
        }
        btn_read.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                selectedItem = item;
                findResource();
            }
        });

        return view;
    }

    @Override
    public int getCount() {
        return items.size();
    }

    @Override
    public ResourceItem getItem(int i) {
        return items.get(i);
    }

    @Override
    public long getItemId(int i) {
        return i;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////

    @Override
    public void onDeviceFound(OcRepresentation ocRepresentation) {
        if(ocRepresentation.getResourceTypes().contains(selectedItem.getDeviceType())) {
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
        if(ocResource != null) {
            String uri = selectedItem.getUri();
            if(uri.indexOf('?') > -1) { uri = uri.substring(0, uri.indexOf('?')); }

            if(ocResource.getUri().equals(uri)) {
                Log.append("Founded resource: " + ocResource.getUri());
                if(ocResource.getAllHosts().size() > 0) {
                    Log.append("access : " + ocResource.getAllHosts().get(0));
                    ocResource.setHost(ocResource.getAllHosts().get(0));
                } else {
                    Log.append("access : " + ocResource.getHost());
                }
                if(selectedItem.getMethod().compareTo("GET") == 0) {
                    Log.append("access GET");
                    try {
                        ocResource.get(selectedItem.getParams(), this);
                    } catch (OcException e) {
                        Log.append("Error occurred while invoking \"get\" API");
                    }
                } else if(selectedItem.getMethod().compareTo("POST") == 0) {
                    Log.append("access POST");
                    OcRepresentation rep = new OcRepresentation();

                    try {
                        switch(selectedItem.getUri()) {
                            case "/ClockResURI":
                                rep.setValue("id", "send_unique_user_id");
                                rep.setValue("datetime", "2017-01-01T12:00:00+0000");
                                rep.setValue("countdown", (double)5.0);
                                break;
                            case "/HeightResURI":
                                rep.setValue("id", "unique_example_id");
                                rep.setValue("height", (double)1.8);
                                rep.setValue("unit", "m");
                                break;
                            case "/UserInfoResURI":
                                rep.setValue("gender", 1);
                                rep.setValue("age", 17);
                                break;
                            default:
                                break;
                        }
                    } catch (OcException e) {
                        Log.append("Failed to make OcRepresentation for POST request");
                    }

                    try {
                        ocResource.post(rep, selectedItem.getParams(), this);
                    } catch (OcException e) {
                        Log.append("Error occurred while invoking \"post\" API");
                    }
                } else if(selectedItem.getMethod().compareTo("OBSERVE") == 0) {
                    try {
                        ocResource.observe(ObserveType.OBSERVE, new HashMap<String, String>(), this);
                    } catch (OcException e) {
                        Log.append("Error occurred while invoking \"observe\" API");
                    }
                } else if(selectedItem.getMethod().compareTo("DEOBSERVE") == 0) {
                    try {
                        ocResource.cancelObserve();
                    } catch (OcException e) {
                        Log.append("Error occurred while invoking \"cancel observe\" API");
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
        ResponseParser parser = new ResponseParser();

        switch(selectedItem.getUri()) {
            case "/oic/d":
                obj_string = parser.parseDeviceInfo(ocRepresentation).toString();
                break;
            case "/oic/p":
                obj_string = parser.parsePlatformInfo(ocRepresentation).toString();
                break;

            ///////////////////////////////////////////////////////////////////////////////////////
            // Thermometer
            ///////////////////////////////////////////////////////////////////////////////////////
            case "/BodyThermometerAMResURI?if=oic.if.baseline":
                obj_string = parser.parseBodyThermometerAMResURI_Baseline(ocRepresentation).toString();
                break;
            case "/BodyThermometerAMResURI?if=oic.if.ll":
                obj_string = parser.parseBodyThermometerAMResURI_LL(ocRepresentation).toString();
                break;
            case "/BodyThermometerAMResURI?if=oic.if.b":
                obj_string = parser.parseBodyThermometerAMResURI_B(ocRepresentation).toString();
                break;
            case "/BodyTemperatureBaselineResURI":
                obj_string = parser.parseBodyTemperatureBaselineResURI(ocRepresentation).toString();
                break;
            case "/BodyTemperatureLLResURI":
                obj_string = parser.parseBodyTemperatureLLResURI(ocRepresentation).toString();
                break;
            case "/BodyTemperatureBatchResURI":
                obj_string = parser.parseBodyTemperatureBatchResURI(ocRepresentation).toString();
                break;
            case "/BodyLocationResURI":
                obj_string = parser.parseBodyLocationResURI(ocRepresentation).toString();
                break;
            case "/BodyLocationTemperatureResURI":
                obj_string = parser.parseBodyLocationTemperatureResURI(ocRepresentation).toString();
                break;

            ///////////////////////////////////////////////////////////////////////////////////////
            // Blood Pressure Monitor
            ///////////////////////////////////////////////////////////////////////////////////////
            case "/BloodPressureMonitorAMResURI?if=oic.if.b":
                String a = "abcdefg";
                obj_string = parser.parseBloodPressureMonitorAMResURI_B(ocRepresentation).toString();
                break;
            case "/BloodPressureMonitorAMResURI?if=oic.if.ll":
                obj_string = parser.parseBloodPressureMonitorAMResURI_LL(ocRepresentation).toString();
                break;
            case "/BloodPressureMonitorAMResURI?if=oic.if.baseline":
                obj_string = parser.parseBloodPressureMonitorAMResURI_Baseline(ocRepresentation).toString();
                break;
            case "/BloodPressureRunResURI":
                obj_string = parser.parseBloodPressureRunResURI(ocRepresentation).toString();
                break;
            case "/BloodPressureResURI":
                obj_string = parser.parseBloodPressureResURI(ocRepresentation).toString();
                break;
            case "/PulseRateResURI":
                obj_string = parser.parsePulseRateResURI(ocRepresentation).toString();
                break;

            ///////////////////////////////////////////////////////////////////////////////////////
            // Scale
            ///////////////////////////////////////////////////////////////////////////////////////
            case "/BodyScaleAMResURI?if=oic.if.b":
                obj_string = parser.parseBodyScaleAMResURI_B(ocRepresentation).toString();
                break;
            case "/BodyScaleAMResURI?if=oic.if.ll":
                obj_string = parser.parseBodyScaleAMResURI_LL(ocRepresentation).toString();
                break;
            case "/BodyScaleAMResURI?if=oic.if.baseline":
                obj_string = parser.parseBodyScaleAMResURI_Baseline(ocRepresentation).toString();
                break;
            case "/ScaleRunResURI":
                obj_string = parser.parseScaleRunResURI(ocRepresentation).toString();
                break;
            case "/BMIResURI":
                obj_string = parser.parseBMIResURI(ocRepresentation).toString();
                break;
            case "/BodyFatResURI":
                obj_string = parser.parseBodyFatResURI(ocRepresentation).toString();
                break;
            case "/BodyFatFreeMassResURI":
                obj_string = parser.parseBodyFatFreeMassResURI(ocRepresentation).toString();
                break;
            case "/BodySoftLeanMassResURI":
                obj_string = parser.parseBodySoftLeanMassResURI(ocRepresentation).toString();
                break;
            case "/BodyWaterResURI":
                obj_string = parser.parseBodyWaterResURI(ocRepresentation).toString();
                break;
            case "/HeightResURI":
                obj_string = parser.parseHeightResURI(ocRepresentation).toString();
                break;
            case "/WeightResURI":
                obj_string = parser.parseWeightResURI(ocRepresentation).toString();
                break;
            case "/UserInfoResURI":
                obj_string = parser.parseUserInfoResURI(ocRepresentation).toString();
                break;

            ///////////////////////////////////////////////////////////////////////////////////////
            // Glucose Meter
            ///////////////////////////////////////////////////////////////////////////////////////
            case "/GlucoseMeterAMResURI?if=oic.if.b":
                obj_string = parser.parseGlucoseMeterAMResURI_B(ocRepresentation).toString();
                break;
            case "/GlucoseMeterAMResURI?if=oic.if.ll":
                obj_string = parser.parseGlucoseMeterAMResURI_LL(ocRepresentation).toString();
                break;
            case "/GlucoseMeterAMResURI?if=oic.if.baseline":
                obj_string = parser.parseGlucoseMeterAMResURI_Baseline(ocRepresentation).toString();
                break;
            case "/DurationResURI":
                obj_string = parser.parseDurationResURI(ocRepresentation).toString();
                break;
            case "/GlucoseRunResURI":
                obj_string = parser.parseGlucoseRunResURI(ocRepresentation).toString();
                break;
            case "/GlucoseResURI":
                obj_string = parser.parseGlucoseResURI(ocRepresentation).toString();
                break;
            case "/GlucoseCarbResURI":
                obj_string = parser.parseGlucoseCarbResURI(ocRepresentation).toString();
                break;
            case "/ExerciseResURI":
                obj_string = parser.parseExerciseResURI(ocRepresentation).toString();
                break;
            case "/GlucoseHbA1cResURI":
                obj_string = parser.parseGlucoseHbA1cResURI(ocRepresentation).toString();
                break;
            case "/GlucoseHealthResURI":
                obj_string = parser.parseGlucoseHealthResURI(ocRepresentation).toString();
                break;
            case "/GlucoseMealResURI":
                obj_string = parser.parseGlucoseMealResURI(ocRepresentation).toString();
                break;
            case "/GlucoseMedicationResURI":
                obj_string = parser.parseGlucoseMedicationResURI(ocRepresentation).toString();
                break;
            case "/GlucoseSampleLocationResURI":
                obj_string = parser.parseGlucoseSampleLocationResURI(ocRepresentation).toString();
                break;
            case "/GlucoseTesterResURI":
                obj_string = parser.parseGlucoseTesterResURI(ocRepresentation).toString();
                break;

            ///////////////////////////////////////////////////////////////////////////////////////
            // Smart Watch
            ///////////////////////////////////////////////////////////////////////////////////////
            case "/UserIDResURI":
                obj_string = parser.parseUserIDResURI(ocRepresentation).toString();
                break;
            case "/ClockResURI":
                obj_string = parser.parseClockResURI(ocRepresentation).toString();
                break;
            case "/HeartrateResURI":
                obj_string = parser.parseHeartrateResURI(ocRepresentation).toString();
                break;

            ///////////////////////////////////////////////////////////////////////////////////////
            default:
                obj_string = new JSONObject().toString();
                break;
        }

        Log.append(obj_string);

        fragment.getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                fragment.setLog(selectedItem, obj_string);
            }
        });
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
    public synchronized void onPostCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation) {
        ResponseParser parser = new ResponseParser();

        switch(selectedItem.getUri()) {
            case "/ClockResURI":
                obj_string = parser.parseClockResURI(ocRepresentation).toString();
                break;
            case "/HeightResURI":
                obj_string = parser.parseHeightResURI(ocRepresentation).toString();
                break;
            case "/UserInfoResURI":
                obj_string = parser.parseUserInfoResURI(ocRepresentation).toString();
                break;
            default:
                obj_string = new JSONObject().toString();
                break;
        }

        Log.append(obj_string);

        fragment.getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                fragment.setLog(selectedItem, obj_string);
            }
        });
    }

    @Override
    public synchronized void onPostFailed(Throwable throwable) {
        if (throwable instanceof OcException) {
            OcException ocEx = (OcException) throwable;
            ErrorCode errCode = ocEx.getErrorCode();

            Log.append("Error: POST representation (errCode: " + errCode + ")\n" + ocEx.getMessage());
        } else {
            Log.append("Error: POST representation");
        }
    }


    /**
     * An event handler to be executed whenever a "post" request completes successfully
     *
     * @param list             list of the header options
     * @param ocRepresentation representation of a resource
     * @param sequenceNumber   sequence number
     */
    @Override
    public synchronized void onObserveCompleted(List<OcHeaderOption> list,
                                                OcRepresentation ocRepresentation,
                                                int sequenceNumber) {

        ResponseParser parser = new ResponseParser();
        obj_string = parser.parseHeartrateResURI(ocRepresentation).toString();

        Log.append(obj_string);

        fragment.getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                fragment.setLog(selectedItem, obj_string);
            }
        });
    }

    /**
     * An event handler to be executed whenever a "observe" request fails
     *
     * @param throwable exception
     */
    @Override
    public synchronized void onObserveFailed(Throwable throwable) {
        if (throwable instanceof OcException) {
            OcException ocEx = (OcException) throwable;
            ErrorCode errCode = ocEx.getErrorCode();

            Log.append("Error: OBSERVE representation (errCode: " + errCode + ")\n" + ocEx.getMessage());
        } else {
            Log.append("Error: OBSERVE representation");
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
}
