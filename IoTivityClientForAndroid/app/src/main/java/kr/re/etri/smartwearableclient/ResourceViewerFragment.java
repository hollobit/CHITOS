package kr.re.etri.smartwearableclient;


import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ListView;
import android.widget.TextView;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

public class ResourceViewerFragment extends Fragment {
    View view;

    ResourceListAdapter resourceListAdapter;
    TextView tv_log;
    ListView lv_resources;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        view = inflater.inflate(R.layout.fragment_resource_list, container, false);

        tv_log = view.findViewById(R.id.tv_log);

        resourceListAdapter = new ResourceListAdapter(getActivity().getApplicationContext(), this);
        lv_resources = view.findViewById(R.id.lv_resources);
        lv_resources.setAdapter(resourceListAdapter);

        return view;
    }

    void setLog(ResourceItem selectedItem, String json_string) {
        String str = "";

        str += "URI: " + selectedItem.getUri() + "\n\n";

        try {
            if(json_string.startsWith("[")) {
                str += new JSONArray(json_string).toString(2).replace("\\/", "/");
            } else {
                str += new JSONObject(json_string).toString(2).replace("\\/", "/");
            }
        } catch(JSONException e) {
            str += "JSONException @ResourceViewerFragment\n" + e.getMessage();
        }

        tv_log.setText(str);
    }
}
