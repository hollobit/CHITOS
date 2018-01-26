package kr.re.etri.smartwearableclient;

import org.iotivity.base.OcException;
import org.iotivity.base.OcRepresentation;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

class ResponseParser {

    JSONObject parseDeviceInfo(OcRepresentation ocRepresentation) {
        return new JSONObject(ocRepresentation.getValues());
    }

    JSONObject parsePlatformInfo(OcRepresentation ocRepresentation) {
        return new JSONObject(ocRepresentation.getValues());
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Thetmometer
    ///////////////////////////////////////////////////////////////////////////////////////////////

    JSONObject parseBodyThermometerAMResURI_Baseline(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            JSONArray ifs = new JSONArray();
            for(String resourceInterface : ocRepresentation.getResourceInterfaces()) {
                ifs.put(resourceInterface);
            }
            ret.put("if", ifs);

            JSONArray rtss = new JSONArray();
            for(String rts_element : (String[]) ocRepresentation.getValues().get("rts")) {
                rtss.put(rts_element);
            }
            ret.put("rts", rtss);

            JSONArray links = new JSONArray();
            for(OcRepresentation subRep : (OcRepresentation[]) ocRepresentation.getValue("links")) {
                JSONObject link = new JSONObject();

                JSONArray rts_ = new JSONArray();
                for (String resourceType : (String[]) subRep.getValue("rt")) {
                    rts_.put(resourceType);
                }
                link.put("rt", rts_);

                JSONArray ifs_ = new JSONArray();
                if(subRep.getValues().containsKey("if")) {
                    for (String resourceInterface : (String[]) subRep.getValue("if")) {
                        ifs_.put(resourceInterface);
                    }
                }
                link.put("if", ifs_);
                link.put("href", subRep.getValue("href"));

                links.put(link);
            }

            ret.put("links", links);
        } catch(OcException e) {
            Log.append("NotFound @parseBodyThermometerAMResURI_Baseline\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseBodyThermometerAMResURI_Baseline\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    JSONArray parseBodyThermometerAMResURI_LL(OcRepresentation ocRepresentation) {
        JSONArray ret = new JSONArray();

        try {
            for(OcRepresentation subRep: ocRepresentation.getChildren()) {
                JSONObject obj = new JSONObject();

                obj.put("href", subRep.getUri());
                obj.put("rt", new JSONArray(subRep.getResourceTypes()));
                obj.put("if", new JSONArray(subRep.getResourceInterfaces()));

                ret.put(obj);
            }
        } catch(JSONException e) {
            Log.append("JSONException @parseBodyThermometerAMResURI_LL\n" + e.getMessage());
            ret = new JSONArray();
        }

        return ret;
    }

    JSONArray parseBodyThermometerAMResURI_B(OcRepresentation ocRepresentation) {
        JSONArray ret = new JSONArray();

        try {
            for(OcRepresentation subRep: ocRepresentation.getChildren()) {
                JSONObject obj = new JSONObject();

                obj.put("href", subRep.getUri());
                obj.put("rep", new JSONObject(((OcRepresentation) subRep.getValue("rep")).getValues()));

                ret.put(obj);
            }
        } catch(OcException e) {
            Log.append("NotFound @parseBodyThermometerAMResURI_B\n" + e.getMessage());
            ret = new JSONArray();
        } catch(JSONException e) {
            Log.append("JSONException @parseBodyThermometerAMResURI_B\n" + e.getMessage());
            ret = new JSONArray();
        }

        return ret;
    }


    JSONObject parseBodyTemperatureBaselineResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            JSONArray ifs = new JSONArray();
            for(String resourceInterface : ocRepresentation.getResourceInterfaces()) {
                ifs.put(resourceInterface);
            }
            ret.put("if", ifs);

            ret.put("id", ocRepresentation.getValue("id"));

            JSONObject ll = parseBodyTemperatureLLResURI(ocRepresentation);
            ret.put("links", ll.get("links"));

        } catch(OcException e) {
            Log.append("NotFound @parseBodyTemperatureBaselineResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseBodyTemperatureBaselineResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    JSONObject parseBodyTemperatureLLResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray links = new JSONArray();
            for(OcRepresentation subRep : (OcRepresentation[]) ocRepresentation.getValue("links")) {
                JSONObject link = new JSONObject();

                JSONArray rts = new JSONArray();
                for (String resourceType : (String[]) subRep.getValue("rt")) {
                    rts.put(resourceType);
                }
                link.put("rt", rts);

                JSONArray ifs = new JSONArray();
                if(subRep.getValues().containsKey("if")) {
                    for (String resourceInterface : (String[]) subRep.getValue("if")) {
                        ifs.put(resourceInterface);
                    }
                }
                link.put("if", ifs);

                link.put("href", subRep.getValue("href"));

                links.put(link);
            }

            ret.put("links", links);

        } catch(OcException e) {
            Log.append("NotFound links @parseBodyTemperatureLLResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseBodyTemperatureLLResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    JSONObject parseBodyTemperatureBatchResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray data_arr = new JSONArray();
            for (OcRepresentation rep : (OcRepresentation[]) ocRepresentation.getValue("data") ) {
                String href = rep.getValue("href");

                JSONObject data = new JSONObject();
                data.put("href", rep.getValue("href"));

                JSONObject _rep = new JSONObject();
                switch(href) {
                    case "/myTemperatureResURI":
                        _rep.put("temperature", ((OcRepresentation) (rep.getValue("rep"))).getValue("temperature"));
                        break;
                    case "/myBodyLocationResURI":
                        _rep.put("bloc", ((OcRepresentation) (rep.getValue("rep"))).getValue("bloc"));
                        break;
                    case "/myClockResURI":
                        _rep.put("datetime", ((OcRepresentation) (rep.getValue("rep"))).getValue("datetime"));
                        break;
                    case "/myUserIDResURI":
                        _rep.put("userid", ((OcRepresentation) (rep.getValue("rep"))).getValue("userid"));
                        break;
                    default:
                        Log.append("NotMatch: href (" + href + ")");
                        break;
                }
                data.put("rep", _rep);

                data_arr.put(data);
            }

            ret.put("data", data_arr);

        } catch(OcException e) {
            Log.append("NotFound data @parseBodyTemperatureBatchResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseBodyTemperatureBatchResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    JSONObject parseBodyLocationResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            ret.put("id", ocRepresentation.getValue("id"));
            ret.put("bloc", ocRepresentation.getValue("bloc"));
        } catch(OcException e) {
            Log.append("NotFound @parseBodyLocationResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseBodyLocationResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    JSONObject parseBodyLocationTemperatureResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            ret.put("id", ocRepresentation.getValue("id"));
            ret.put("bloc", ocRepresentation.getValue("bloc"));
        } catch(OcException e) {
            Log.append("NotFound @parseBodyLocationTemperatureResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseBodyLocationTemperatureResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Blood Pressure Monitor
    ///////////////////////////////////////////////////////////////////////////////////////////////

    JSONArray parseBloodPressureMonitorAMResURI_B(OcRepresentation ocRepresentation) {
        JSONArray ret = new JSONArray();

        try {
            for(OcRepresentation subRep: ocRepresentation.getChildren()) {
                JSONObject obj = new JSONObject();

                obj.put("href", subRep.getUri());
                obj.put("rep", new JSONObject(((OcRepresentation) subRep.getValue("rep")).getValues()));

                ret.put(obj);
            }
        } catch(OcException e) {
            Log.append("NotFound @parseBloodPressureMonitorAMResURI_B\n" + e.getMessage());
            ret = new JSONArray();
        } catch(JSONException e) {
            Log.append("JSONException @parseBloodPressureMonitorAMResURI_B\n" + e.getMessage());
            ret = new JSONArray();
        }

        return ret;
    }

    JSONArray parseBloodPressureMonitorAMResURI_LL(OcRepresentation ocRepresentation) {
        JSONArray ret = new JSONArray();

        try {
            for(OcRepresentation subRep: ocRepresentation.getChildren()) {
                JSONObject obj = new JSONObject();

                obj.put("href", subRep.getUri());
                obj.put("rt", new JSONArray(subRep.getResourceTypes()));
                obj.put("if", new JSONArray(subRep.getResourceInterfaces()));

                ret.put(obj);
            }
        } catch(JSONException e) {
            Log.append("JSONException @parseBloodPressureMonitorAMResURI_LL\n" + e.getMessage());
            ret = new JSONArray();
        }

        return ret;
    }

    JSONObject parseBloodPressureMonitorAMResURI_Baseline(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            JSONArray ifs = new JSONArray();
            for(String resourceInterface : ocRepresentation.getResourceInterfaces()) {
                ifs.put(resourceInterface);
            }
            ret.put("if", ifs);

            JSONArray rtss = new JSONArray();
            for(String rts_element : (String[]) ocRepresentation.getValues().get("rts")) {
                rtss.put(rts_element);
            }
            ret.put("rts", rtss);

            JSONArray links = new JSONArray();
            for(OcRepresentation subRep : (OcRepresentation[]) ocRepresentation.getValue("links")) {
                JSONObject link = new JSONObject();

                JSONArray rts_ = new JSONArray();
                for (String resourceType : (String[]) subRep.getValue("rt")) {
                    rts_.put(resourceType);
                }
                link.put("rt", rts_);

                JSONArray ifs_ = new JSONArray();
                if(subRep.getValues().containsKey("if")) {
                    for (String resourceInterface : (String[]) subRep.getValue("if")) {
                        ifs_.put(resourceInterface);
                    }
                }
                link.put("if", ifs_);
                link.put("href", subRep.getValue("href"));

                links.put(link);
            }

            ret.put("links", links);


        } catch(OcException e) {
            Log.append("NotFound @parseBloodPressureMonitorAMResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseBloodPressureMonitorAMResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    JSONObject parseBloodPressureRunResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            ret.put("id", ocRepresentation.getValue("id"));
            ret.put("result", ocRepresentation.getValue("result"));
        } catch(OcException e) {
            Log.append("NotFound @parseBloodPressureRunResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseBloodPressureRunResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    JSONObject parseBloodPressureResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            ret.put("id", ocRepresentation.getValue("id"));
            ret.put("systolic", ocRepresentation.getValue("systolic"));
            ret.put("diastolic", ocRepresentation.getValue("diastolic"));
            ret.put("unit", ocRepresentation.getValue("unit"));
        } catch(OcException e) {
            Log.append("NotFound data @parseBloodPressureResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseBloodPressureResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    JSONObject parsePulseRateResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            ret.put("id", ocRepresentation.getValue("id"));
            ret.put("pulserate", ocRepresentation.getValue("pulserate"));
        } catch(OcException e) {
            Log.append("NotFound data @parsePulseRateResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parsePulseRateResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Body Scale
    ///////////////////////////////////////////////////////////////////////////////////////////////

    JSONArray parseBodyScaleAMResURI_B(OcRepresentation ocRepresentation) {
        JSONArray ret = new JSONArray();

        try {
            for(OcRepresentation subRep: ocRepresentation.getChildren()) {
                JSONObject obj = new JSONObject();

                obj.put("href", subRep.getUri());
                obj.put("rep", new JSONObject(((OcRepresentation) subRep.getValue("rep")).getValues()));

                ret.put(obj);
            }
        } catch(OcException e) {
            Log.append("NotFound @parseBodyScaleAMResURI_B\n" + e.getMessage());
            ret = new JSONArray();
        } catch(JSONException e) {
            Log.append("JSONException @parseBodyScaleAMResURI_B\n" + e.getMessage());
            ret = new JSONArray();
        }

        return ret;
    }

    JSONArray parseBodyScaleAMResURI_LL(OcRepresentation ocRepresentation) {
        JSONArray ret = new JSONArray();

        try {
            for(OcRepresentation subRep: ocRepresentation.getChildren()) {
                JSONObject obj = new JSONObject();

                obj.put("href", subRep.getUri());
                obj.put("rt", new JSONArray(subRep.getResourceTypes()));
                obj.put("if", new JSONArray(subRep.getResourceInterfaces()));

                ret.put(obj);
            }
        } catch(JSONException e) {
            Log.append("JSONException @parseBodyScaleAMResURI_LL\n" + e.getMessage());
            ret = new JSONArray();
        }

        return ret;
    }

    JSONObject parseBodyScaleAMResURI_Baseline(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            JSONArray ifs = new JSONArray();
            for(String resourceInterface : ocRepresentation.getResourceInterfaces()) {
                ifs.put(resourceInterface);
            }
            ret.put("if", ifs);

            JSONArray rtss = new JSONArray();
            for(String rts_element : (String[]) ocRepresentation.getValues().get("rts")) {
                rtss.put(rts_element);
            }
            ret.put("rts", rtss);

            JSONArray links = new JSONArray();
            for(OcRepresentation subRep : (OcRepresentation[]) ocRepresentation.getValue("links")) {
                JSONObject link = new JSONObject();

                JSONArray rts_ = new JSONArray();
                for (String resourceType : (String[]) subRep.getValue("rt")) {
                    rts_.put(resourceType);
                }
                link.put("rt", rts_);

                JSONArray ifs_ = new JSONArray();
                if(subRep.getValues().containsKey("if")) {
                    for (String resourceInterface : (String[]) subRep.getValue("if")) {
                        ifs_.put(resourceInterface);
                    }
                }
                link.put("if", ifs_);
                link.put("href", subRep.getValue("href"));

                links.put(link);
            }

            ret.put("links", links);
        } catch(OcException e) {
            Log.append("NotFound @parseBodyScaleAMResURI_Baseline\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseBodyScaleAMResURI_Baseline\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    JSONObject parseScaleRunResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            ret.put("id", ocRepresentation.getValue("id"));
            ret.put("result", ocRepresentation.getValue("result"));
        } catch(OcException e) {
            Log.append("NotFound @parseScaleRunResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseScaleRunResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    JSONObject parseBMIResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            ret.put("id", ocRepresentation.getValue("id"));
            ret.put("bmi", ocRepresentation.getValue("bmi"));
        } catch(OcException e) {
            Log.append("NotFound @parseBMIResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseBMIResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    JSONObject parseBodyFatResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            ret.put("id", ocRepresentation.getValue("id"));
            ret.put("bodyfat", ocRepresentation.getValue("bodyfat"));
            ret.put("unit", ocRepresentation.getValue("unit"));
        } catch(OcException e) {
            Log.append("NotFound @parseBodyFatResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseBodyFatResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    JSONObject parseBodyFatFreeMassResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            ret.put("id", ocRepresentation.getValue("id"));
            ret.put("ffm", ocRepresentation.getValue("ffm"));
            ret.put("unit", ocRepresentation.getValue("unit"));
        } catch(OcException e) {
            Log.append("NotFound @parseBodyFatFreeMassResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseBodyFatFreeMassResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    JSONObject parseBodySoftLeanMassResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            ret.put("id", ocRepresentation.getValue("id"));
            ret.put("slm", ocRepresentation.getValue("slm"));
            ret.put("unit", ocRepresentation.getValue("unit"));
        } catch(OcException e) {
            Log.append("NotFound @parseBodySoftLeanMassResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseBodySoftLeanMassResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    JSONObject parseBodyWaterResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            ret.put("id", ocRepresentation.getValue("id"));
            ret.put("bwater", ocRepresentation.getValue("bwater"));
            ret.put("unit", ocRepresentation.getValue("unit"));
        } catch(OcException e) {
            Log.append("NotFound @parseBodyWaterResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseBodyWaterResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    JSONObject parseHeightResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            ret.put("id", ocRepresentation.getValue("id"));
            ret.put("height", ocRepresentation.getValue("height"));
            ret.put("unit", ocRepresentation.getValue("unit"));
        } catch(OcException e) {
            Log.append("NotFound @parseHeightResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseHeightResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    JSONObject parseWeightResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            ret.put("id", ocRepresentation.getValue("id"));
            ret.put("weight", ocRepresentation.getValue("weight"));
            ret.put("unit", ocRepresentation.getValue("unit"));
        } catch(OcException e) {
            Log.append("NotFound data @parseWeightResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseWeightResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Glucose Meter
    ///////////////////////////////////////////////////////////////////////////////////////////////

    JSONArray parseGlucoseMeterAMResURI_B(OcRepresentation ocRepresentation) {
        JSONArray ret = new JSONArray();

        try {
            for(OcRepresentation subRep: ocRepresentation.getChildren()) {
                JSONObject obj = new JSONObject();

                obj.put("href", subRep.getUri());
                obj.put("rep", new JSONObject(((OcRepresentation) subRep.getValue("rep")).getValues()));

                ret.put(obj);
            }
        } catch(OcException e) {
            Log.append("NotFound @parseGlucoseMeterAMResURI_B\n" + e.getMessage());
            ret = new JSONArray();
        } catch(JSONException e) {
            Log.append("JSONException @parseGlucoseMeterAMResURI_B\n" + e.getMessage());
            ret = new JSONArray();
        }

        return ret;
    }

    JSONArray parseGlucoseMeterAMResURI_LL(OcRepresentation ocRepresentation) {
        JSONArray ret = new JSONArray();

        try {
            for(OcRepresentation subRep: ocRepresentation.getChildren()) {
                JSONObject obj = new JSONObject();

                obj.put("href", subRep.getUri());
                obj.put("rt", new JSONArray(subRep.getResourceTypes()));
                obj.put("if", new JSONArray(subRep.getResourceInterfaces()));

                ret.put(obj);
            }
        } catch(JSONException e) {
            Log.append("JSONException @parseGlucoseMeterAMResURI_LL\n" + e.getMessage());
            ret = new JSONArray();
        }

        return ret;
    }

    JSONObject parseGlucoseMeterAMResURI_Baseline(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            JSONArray ifs = new JSONArray();
            for(String resourceInterface : ocRepresentation.getResourceInterfaces()) {
                ifs.put(resourceInterface);
            }
            ret.put("if", ifs);

            JSONArray rtss = new JSONArray();
            for(String rts_element : (String[]) ocRepresentation.getValues().get("rts")) {
                rtss.put(rts_element);
            }
            ret.put("rts", rtss);

            JSONArray links = new JSONArray();
            for(OcRepresentation subRep : (OcRepresentation[]) ocRepresentation.getValue("links")) {
                JSONObject link = new JSONObject();

                JSONArray rts_ = new JSONArray();
                for (String resourceType : (String[]) subRep.getValue("rt")) {
                    rts_.put(resourceType);
                }
                link.put("rt", rts_);

                JSONArray ifs_ = new JSONArray();
                if(subRep.getValues().containsKey("if")) {
                    for (String resourceInterface : (String[]) subRep.getValue("if")) {
                        ifs_.put(resourceInterface);
                    }
                }
                link.put("if", ifs_);
                link.put("href", subRep.getValue("href"));

                links.put(link);
            }

            ret.put("links", links);
        } catch(OcException e) {
            Log.append("NotFound @parseGlucoseMeterAMResURI_Baseline\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseGlucoseMeterAMResURI_Baseline\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    JSONObject parseDurationResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            ret.put("rt", new JSONArray(ocRepresentation.getResourceTypes()));
            ret.put("id", ocRepresentation.getValue("id"));
            ret.put("duration", ocRepresentation.getValue("duration"));
        } catch(OcException e) {
            Log.append("NotFound @parseDurationResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseDurationResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    JSONObject parseGlucoseRunResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            ret.put("id", ocRepresentation.getValue("id"));
            ret.put("result", ocRepresentation.getValue("result"));
        } catch(OcException e) {
            Log.append("NotFound data @parseGlucoseRunResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseGlucoseRunResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    JSONObject parseGlucoseResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            ret.put("id", ocRepresentation.getValue("id"));
            ret.put("glucose", ocRepresentation.getValue("glucose"));
            ret.put("unit", ocRepresentation.getValue("unit"));
        } catch(OcException e) {
            Log.append("NotFound data @parseGlucoseResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseGlucoseResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    JSONObject parseGlucoseCarbResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            ret.put("id", ocRepresentation.getValue("id"));
            ret.put("carb", ocRepresentation.getValue("carb"));
            ret.put("meal", ocRepresentation.getValue("meal"));
        } catch(OcException e) {
            Log.append("NotFound @parseGlucoseCarbResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseGlucoseCarbResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    JSONObject parseExerciseResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            ret.put("id", ocRepresentation.getValue("id"));
            ret.put("exercise", ocRepresentation.getValue("exercise"));
        } catch(OcException e) {
            Log.append("NotFound @parseGlucoseExerciseResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseGlucoseExerciseResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    JSONObject parseGlucoseHbA1cResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            ret.put("id", ocRepresentation.getValue("id"));
            ret.put("hba1c", ocRepresentation.getValue("hba1c"));
        } catch(OcException e) {
            Log.append("NotFound @parseGlucoseHbA1cResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseGlucoseHbA1cResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    JSONObject parseGlucoseHealthResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            ret.put("id", ocRepresentation.getValue("id"));
            ret.put("health", ocRepresentation.getValue("health"));
        } catch(OcException e) {
            Log.append("NotFound @parseGlucoseHealthResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseGlucoseHealthResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    JSONObject parseGlucoseMealResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            ret.put("id", ocRepresentation.getValue("id"));
            ret.put("meal", ocRepresentation.getValue("meal"));
        } catch(OcException e) {
            Log.append("NotFound @parseGlucoseMealResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseGlucoseMealResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    JSONObject parseGlucoseMedicationResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            ret.put("id", ocRepresentation.getValue("id"));
            ret.put("medication", ocRepresentation.getValue("medication"));
            ret.put("unit", ocRepresentation.getValue("unit"));
            ret.put("regimen", ocRepresentation.getValue("regimen"));
        } catch(OcException e) {
            Log.append("NotFound @parseGlucoseMedicationResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseGlucoseMedicationResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    JSONObject parseGlucoseSampleLocationResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            ret.put("id", ocRepresentation.getValue("id"));
            ret.put("samplelocation", ocRepresentation.getValue("samplelocation"));
        } catch(OcException e) {
            Log.append("NotFound @parseGlucoseSampleLocationResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseGlucoseSampleLocationResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    JSONObject parseGlucoseTesterResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            ret.put("id", ocRepresentation.getValue("id"));
            ret.put("tester", ocRepresentation.getValue("tester"));
        } catch(OcException e) {
            Log.append("NotFound @parseGlucoseTesterResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseGlucoseTesterResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Smart Watch
    ///////////////////////////////////////////////////////////////////////////////////////////////

    JSONObject parseUserIDResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            ret.put("id", ocRepresentation.getValue("id"));
            ret.put("userid", ocRepresentation.getValue("userid"));
        } catch(OcException e) {
            Log.append("NotFound @parseUserIDResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseUserIDResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    JSONObject parseClockResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            ret.put("id", ocRepresentation.getValue("id"));
            ret.put("datetime", ocRepresentation.getValue("datetime"));
            ret.put("countdown", ocRepresentation.getValue("countdown"));
        } catch(OcException e) {
            Log.append("NotFound @parseClockResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseClockResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    JSONObject parseUserInfoResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            ret.put("gender", ocRepresentation.getValue("gender"));
            ret.put("age", ocRepresentation.getValue("age"));
        } catch(OcException e) {
            Log.append("NotFound @parseUserInfoResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseUserInfoResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }

    JSONObject parseHeartrateResURI(OcRepresentation ocRepresentation) {
        JSONObject ret = new JSONObject();

        try {
            JSONArray rts = new JSONArray();
            for(String resourceType : ocRepresentation.getResourceTypes()) {
                rts.put(resourceType);
            }
            ret.put("rt", rts);

            ret.put("id", ocRepresentation.getValue("id"));
            ret.put("heartrate", ocRepresentation.getValue("heartrate"));
        } catch(OcException e) {
            Log.append("NotFound @parseHeartrateResURI\n" + e.getMessage());
            ret = new JSONObject();
        } catch(JSONException e) {
            Log.append("JSONException @parseHeartrateResURI\n" + e.getMessage());
            ret = new JSONObject();
        }

        return ret;
    }
}
