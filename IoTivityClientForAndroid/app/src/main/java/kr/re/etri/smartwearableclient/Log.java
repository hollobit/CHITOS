package kr.re.etri.smartwearableclient;

import java.util.ArrayList;
import java.util.List;

class Log {
    private static List<LogItem> list = new ArrayList<>();

    static void append(String msg) {
        list.add(new LogItem(msg));
        e("CLIENT", msg);
    }

    static List<LogItem> getList() {
        return list;
    }

    static void clear() {
        list.clear();
    }

    static int e(String tag, String msg) {
        return android.util.Log.e(tag, msg);
    }
}
