package kr.re.etri.smartwearableclient;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import java.util.List;

public class LogViewerFragment extends Fragment {
    View view;

    public LogViewerFragment() {
    }


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        view = inflater.inflate(R.layout.fragment_log, container, false);

        final TextView tv_log = view.findViewById(R.id.tv_log);
        tv_log.setText("");

        Button btn_clear = view.findViewById(R.id.btn_clear);
        btn_clear.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Log.clear();
                tv_log.setText("");
            }
        });

        List<LogItem> list = Log.getList();
        for(int i = 0; i < list.size(); i++) {
            LogItem item = list.get(list.size() - i - 1);

            tv_log.append(String.format("[%s] %s\n", item.getTime(), item.getMsg()));
        }

        return view;
    }
}
