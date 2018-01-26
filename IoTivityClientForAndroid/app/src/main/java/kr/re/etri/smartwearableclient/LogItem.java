package kr.re.etri.smartwearableclient;

import java.text.SimpleDateFormat;
import java.util.Date;

class LogItem {
    private String msg;
    private String time;

    LogItem(String msg) {
        SimpleDateFormat sdf = new SimpleDateFormat("hh:mm:ss");

        this.msg = msg;
        this.time = sdf.format(new Date());
    }

    String getTime() {
        return time;
    }

    String getMsg() {
        return msg;
    }
}
