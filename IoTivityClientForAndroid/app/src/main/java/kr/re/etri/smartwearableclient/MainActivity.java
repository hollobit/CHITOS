package kr.re.etri.smartwearableclient;

import android.content.DialogInterface;
import android.graphics.Color;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentTransaction;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity {
    private static final int BUFFER_SIZE = 1024;
    private static final String OIC_CLIENT_CBOR_DB_FILE = "oic_svr_db_client_devowner.dat";

    private String filePath = "";
    private String fullPath = "";

    private DrawerLayout drawer_layout;
    private ActionBarDrawerToggle drawer_toggle;
    private ListView lv_navigator;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        drawer_layout = (DrawerLayout) findViewById(R.id.drawer_layout);

        List<NavigatorItem> navi_item_list = new ArrayList<>();
        navi_item_list.add(new NavigatorItem("Thermometer"));
        navi_item_list.add(new NavigatorItem("Blood Pressure Meter"));
        navi_item_list.add(new NavigatorItem("Glucose Meter"));
        navi_item_list.add(new NavigatorItem("Scale"));
        navi_item_list.add(new NavigatorItem("Smart Watch"));
        navi_item_list.add(new NavigatorItem("Resource Viewer"));
        navi_item_list.add(new NavigatorItem("Log Viewer"));
        navi_item_list.add(new NavigatorItem("About"));

        lv_navigator = (ListView) findViewById(R.id.lv_navigator);
        lv_navigator.setAdapter(new NavigatorAdapter(getApplicationContext(), navi_item_list));
        lv_navigator.setOnItemClickListener(nav_item_onClickListener);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        toolbar.setTitleTextColor(Color.WHITE);
        setSupportActionBar(toolbar);

        drawer_toggle = new ActionBarDrawerToggle(this, drawer_layout, toolbar, R.string.app_name, R.string.app_name) {
            @Override
            public void onDrawerOpened(View drawerView) {
                super.onDrawerOpened(drawerView);
            }

            @Override
            public void onDrawerClosed(View drawerView) {
                super.onDrawerClosed(drawerView);
            }
        };

        drawer_layout.addDrawerListener(drawer_toggle);
        drawer_toggle.syncState();

        iotivityConfigure();

        changeFragment(new ResourceViewerFragment());
    }

    private ListView.OnItemClickListener nav_item_onClickListener = new ListView.OnItemClickListener() {
        @Override
        public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
            drawer_layout.closeDrawer(lv_navigator);

            switch(i) {
                case 0:
                    changeFragment(new ThermometerFragment());
                    break;
                case 1:
                    changeFragment(new BpmFragment());
                    break;
                case 2:
                    changeFragment(new GlucoseMeterFragment());
                    break;
                case 3:
                    changeFragment(new ScaleFragment());
                    break;
                case 4:
                    changeFragment(new SmartwatchFragment());
                    break;
                case 5:
                    changeFragment(new ResourceViewerFragment());
                    break;
                case 6:
                    changeFragment(new LogViewerFragment());
                    break;
                case 7:
                    changeFragment(new InfoFragment());
                    break;
                default:
                    break;
            }
        }
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////

    void iotivityConfigure() {
        Log.append("IoTivity client start");
        copyCborFromAsset();
        PlatformConfig platformConfig = new PlatformConfig(
                this,
                getApplicationContext(),
                ServiceType.IN_PROC,
                ModeType.CLIENT_SERVER,
                "0.0.0.0", // By setting to "0.0.0.0", it binds to all available interfaces
                0,         // Uses randomly available port
                QualityOfService.LOW,
                fullPath
        );

        OcPlatform.Configure(platformConfig);
        Log.append("IoTivity initialized");
    }

    private void copyCborFromAsset() {
        filePath = getFilesDir().getPath() + "/"; //  data/data/<package>/files/
        InputStream inputStream = null;
        OutputStream outputStream = null;
        int length;
        byte[] buffer = new byte[BUFFER_SIZE];
        try {
            inputStream = getAssets().open(OIC_CLIENT_CBOR_DB_FILE);
            File file = new File(filePath);
            // check files directory exists
            if (!(file.exists() && file.isDirectory())) {
                file.mkdirs();
            }
            fullPath = filePath + OIC_CLIENT_CBOR_DB_FILE;
            outputStream = new FileOutputStream(fullPath);
            while ((length = inputStream.read(buffer)) != -1) {
                outputStream.write(buffer, 0, length);
            }
        } catch (NullPointerException e) {
            Log.append("Error: Null pointer exception @ copyCborFromAsset\n" + e.getMessage());
        } catch (FileNotFoundException e) {
            Log.append("Error: CBOR svr db file not found @ copyCborFromAsset\n" + e.getMessage());
        } catch (IOException e) {
            Log.append("Error: file copy failed (" + OIC_CLIENT_CBOR_DB_FILE + ") from copyCborFromAseet\n" + e.getMessage());
        } finally {
            if (inputStream != null) {
                try {
                    inputStream.close();
                } catch (IOException e) {
                    Log.append("Error: inputStream @ copyCborFromAsset\n" + e.getMessage());
                }
            }
            if (outputStream != null) {
                try {
                    outputStream.close();
                } catch (IOException e) {
                    Log.append("Error: outputStream @ copyCborFromAsset\n" + e.getMessage());
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////

    @Override
    public void onBackPressed() {
        if(getSupportFragmentManager().getBackStackEntryCount() == 0) {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle("Confirm");
            builder.setMessage("Are you sure you want to exit?");
            builder.setPositiveButton("Yes", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialogInterface, int i) {
                    System.exit(0);
                }
            });
            builder.setNegativeButton("No", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialogInterface, int i) {
                    dialogInterface.dismiss();
                }
            });
            builder.show();
        } else {
            super.onBackPressed();
        }
    }

    private void changeFragment(Fragment fragment) {
        changeFragment(fragment, false);
    }

    private void changeFragment(Fragment fragment, boolean isAddToBackStack) {
        FragmentTransaction ft = getSupportFragmentManager().beginTransaction();
        ft.replace(R.id.fragment_container, fragment);
        if(isAddToBackStack) {
            ft.addToBackStack(null);
        }
        ft.commit();
    }

}
