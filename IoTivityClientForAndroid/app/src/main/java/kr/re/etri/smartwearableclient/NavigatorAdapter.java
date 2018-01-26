package kr.re.etri.smartwearableclient;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;

class NavigatorAdapter extends BaseAdapter {
    private Context context;
    private List<NavigatorItem> items = new ArrayList<>();

    NavigatorAdapter(Context context, List<NavigatorItem> items) {
        this.context = context;
        this.items = items;

        notifyDataSetChanged();
    }

    @Override
    public View getView(int i, View view, ViewGroup viewGroup) {
        NavigatorItem item = (NavigatorItem) getItem(i);

        if(view == null) {
            LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            view = inflater.inflate(R.layout.list_item_navigator, viewGroup, false);
        }

        TextView tv_title = view.findViewById(R.id.tv_title);
        tv_title.setText(item.getTitle());

        return view;
    }

    @Override
    public Object getItem(int i) {
        return items.get(i);
    }

    @Override
    public int getCount() {
        return items.size();
    }

    @Override
    public long getItemId(int i) {
        return i;
    }
}
