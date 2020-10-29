package com.example.mdpgroup16;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;
import androidx.localbroadcastmanager.content.LocalBroadcastManager;
import androidx.viewpager.widget.ViewPager;

import android.Manifest;
import android.app.Dialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Color;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.widget.ListView;
import android.widget.Toast;
import android.widget.TextView;

import androidx.appcompat.widget.Toolbar;

import com.google.android.material.tabs.TabItem;
import com.google.android.material.tabs.TabLayout;

import java.util.ArrayList;
import java.util.UUID;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import android.os.SystemClock;
import android.widget.ToggleButton;

public class MainActivity extends AppCompatActivity implements SensorEventListener {

    private static final String TAG = "MainActivity";

    private SensorManager sensorManager;
    Sensor accelerometer;

    public static final int RECONNECT_MAXIMUM_TIMES = 5;
    public static final String SHARED_PREFS = "sharedPrefs";
    public static final String TEXT_F1 = "textF1";
    public static final String TEXT_F2 = "textF2";

    private TabLayout tabLayout;
    private ViewPager viewPager;
    private Toolbar bluetoothToolBar;
    private TextView bluetoothToolBarText;

    private int[] tabIcons = {
            R.drawable.tab_map,
            R.drawable.tab_bluetooth
    };
    private TabItem tab1, tab2, tab3;
    public PageAdapter pageradapter;

    private int reconnectCount = 0;

    BluetoothAdapter mBluetoothAdapter;
    BluetoothConnectionService mBluetoothConnection;

    StringBuilder messages, mdf_messages, nid_messages;

    private static final UUID MY_UUID_INSECURE = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    BluetoothDevice mBTDevice;

    public ArrayList<BluetoothDevice> mBTDevices = new ArrayList<>();
    public ArrayList<BluetoothDevice> mBTNEWDevices = new ArrayList<>();

    public DeviceListAdapter mDeviceListAdapter;

    ListView lvNewDevices;
    ListView lstvw;



    public static boolean wayPointChecked = false;
    public static boolean tiltChecked = false;

    Dialog myReconfigureDialog;

    private void setupTabIcons() {
        tabLayout.getTabAt(0).setIcon(tabIcons[0]);
        tabLayout.getTabAt(1).setIcon(tabIcons[1]);
    }

    /**
     * Broadcast Receiver for listing devices that are not yet paired
     * -Executed by btnDiscover() method.
     */
    private BroadcastReceiver mBroadcastReceiver3 = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            //Log.d(TAG, "onReceive: ACTION FOUND.");

            if (action.equals(BluetoothDevice.ACTION_FOUND)) {
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                mBTDevices.add(device);
                /*if (device.getAddress().contains("B8:27:EB:67:AA:2A")) {
                    mBTDevices.add(device);
                }*/
                //Log.d(TAG, "onReceive: " + device.getName() + ": " + device.getAddress());
                mDeviceListAdapter = new DeviceListAdapter(context, R.layout.device_adapter_view, mBTDevices);
                lvNewDevices.setAdapter(mDeviceListAdapter);
            }
        }
    };
    private BroadcastReceiver mBroadcastReceiver13 = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            //Log.d(TAG, "onReceive: ACTION FOUND.");

            if (action.equals(BluetoothDevice.ACTION_FOUND)) {
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                //mBTDevices.add(device);
                if (device.getAddress().contains("B8:27:EB:12:0D:6F")) {
                    mBTNEWDevices.add(device);

                    //Log.d(TAG, "onReceive: " + device.getName() + ": " + device.getAddress());
                    mDeviceListAdapter = new DeviceListAdapter(context, R.layout.device_adapter_view, mBTNEWDevices);
                    lstvw.setAdapter(mDeviceListAdapter);}
                if (device.getAddress().contains("14:AB:C5:8D:DC:94")) {
                    mBTNEWDevices.add(device);

                    //Log.d(TAG, "onReceive: " + device.getName() + ": " + device.getAddress());
                    mDeviceListAdapter = new DeviceListAdapter(context, R.layout.device_adapter_view, mBTNEWDevices);
                    lstvw.setAdapter(mDeviceListAdapter);}
            }
        }
    };

    /**
     * Broadcast Receiver that detects bond state changes (Pairing status changes)
     */
    private final BroadcastReceiver mBroadcastReceiver4 = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();

            if (action.equals(BluetoothDevice.ACTION_BOND_STATE_CHANGED)) {
                BluetoothDevice mDevice = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                //3 cases:
                //case1: bonded already
                if (mDevice.getBondState() == BluetoothDevice.BOND_BONDED) {
                    Log.d(TAG, "BroadcastReceiver: BOND_BONDED.");
                    //inside BroadcastReceiver4
                    mBTDevice = mDevice;
                }
                //case2: creating a bond
                if (mDevice.getBondState() == BluetoothDevice.BOND_BONDING) {
                    Log.d(TAG, "BroadcastReceiver: BOND_BONDING.");
                }
                //case3: breaking a bond
                if (mDevice.getBondState() == BluetoothDevice.BOND_NONE) {
                    //Log.d(TAG, "BroadcastReceiver: BOND_NONE.");
                }
            }
        }
    };

    /**
     * Broadcast Receiver for listing devices that are listen for disconnection
     */
    private BroadcastReceiver mBroadcastReceiver2 = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();

            if (BluetoothDevice.ACTION_ACL_CONNECTED.equals(action)) {
                showBluetoothConnected();
            }
            if (BluetoothDevice.ACTION_ACL_DISCONNECTED.equals(action)) {
                //Device has disconnected
                //Log.d(TAG, "Device Disconnected");
                //reconnect();

                showBluetoothDisconnected();

                // reconnect to the device 5 times
                if (reconnectCount < RECONNECT_MAXIMUM_TIMES) {
                    // wait 3second before calling the reconnect function
                    new Handler().postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            reconnect();
                        }
                    }, 3000);
                }
            }
        }
    };

    private void reconnect() {
        startConnection();

        if (mBluetoothConnection.getBluetoothState()) {
            reconnectCount = 0;
        } else {
            reconnectCount++;
        }
    }


    @Override
    protected void onDestroy() {
        //Log.d(TAG, "onDestroy: called.");
        super.onDestroy();
        unregisterReceiver(mBroadcastReceiver2);
        unregisterReceiver(mBroadcastReceiver3);
        unregisterReceiver(mBroadcastReceiver4);
        unregisterReceiver(mBroadcastReceiver13);
        //mBluetoothAdapter.cancelDiscovery();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.tabs);

        // For tablayout
        tabLayout = (TabLayout) findViewById(R.id.tablayout);
        tab1 = (TabItem) findViewById(R.id.Tab1);
        tab2 = (TabItem) findViewById(R.id.Tab2);
        tab3 = (TabItem) findViewById(R.id.Tab3);
        viewPager = findViewById(R.id.viewpager);

        // For bluetooth status bar
        bluetoothToolBar = (Toolbar) findViewById(R.id.btToolbar);
        bluetoothToolBarText = (TextView) findViewById(R.id.bluetoothTextView);

        messages = new StringBuilder();
        mdf_messages = new StringBuilder();
        nid_messages = new StringBuilder();

        myReconfigureDialog = new Dialog(this);

        pageradapter = new PageAdapter(getSupportFragmentManager(), tabLayout.getTabCount());
        viewPager.setAdapter(pageradapter);

        tabLayout.addOnTabSelectedListener(new TabLayout.OnTabSelectedListener() {
            @Override
            public void onTabSelected(TabLayout.Tab tab) {
                viewPager.setCurrentItem(tab.getPosition());
                if (tab.getPosition() == 0) {
                    pageradapter.notifyDataSetChanged();
                } else if (tab.getPosition() == 1) {
                    pageradapter.notifyDataSetChanged();
                } else if (tab.getPosition() == 2) {
                    pageradapter.notifyDataSetChanged();
                }
            }

            @Override
            public void onTabUnselected(TabLayout.Tab tab) {

            }

            @Override
            public void onTabReselected(TabLayout.Tab tab) {

            }
        });

        viewPager.addOnPageChangeListener(new TabLayout.TabLayoutOnPageChangeListener(tabLayout));

        setupTabIcons();

        // bluetooth variables
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        mBTDevices = new ArrayList<>();
        mBTNEWDevices = new ArrayList<>();

        //Broadcasts when bond state changes (ie:pairing)
        IntentFilter filter = new IntentFilter(BluetoothDevice.ACTION_BOND_STATE_CHANGED);
        registerReceiver(mBroadcastReceiver4, filter);

        //Broadcast when disconnected
        IntentFilter disconnectedDevicesIntent = new IntentFilter(BluetoothDevice.ACTION_ACL_DISCONNECTED);
        registerReceiver(mBroadcastReceiver2, disconnectedDevicesIntent);

        //Broadcast when connected
        IntentFilter connectedDevicesIntent = new IntentFilter(BluetoothDevice.ACTION_ACL_CONNECTED);
        registerReceiver(mBroadcastReceiver2, connectedDevicesIntent);


        // Set up broadcast for receiving message
        LocalBroadcastManager.getInstance(this).registerReceiver(mReceiver, new IntentFilter("incomingMessage"));

        sensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        accelerometer = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        sensorManager.registerListener(MainActivity.this, accelerometer, SensorManager.SENSOR_DELAY_NORMAL);

    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int i){

    }

    @Override
    public void onSensorChanged(SensorEvent sensorEvent){
        float xtilt;
        float ytilt;
        xtilt = sensorEvent.values[0];
        ytilt = sensorEvent.values[1];
        ((MapTab) pageradapter.fragment1).setIncomingX(xtilt);
        ((MapTab) pageradapter.fragment1).setIncomingY(ytilt);
        //Log.d(TAG, "X value: " + sensorEvent.values[0] + "Y value: " + sensorEvent.values[1] );
    }

    //create method for starting connection
//***remember the conncction will fail and app will crash if you haven't paired first
    public void startConnection() {
        startBTConnection(mBTDevice, MY_UUID_INSECURE);
    }

    /**
     * starting chat service method
     */
    public void startBTConnection(BluetoothDevice device, UUID uuid) {
        //Log.d(TAG, "startBTConnection: Initializing RFCOM Bluetooth Connection.");
        mBluetoothConnection.startClient(device, uuid);
    }


    //Added the UI for Bluetooth
    //Call it in the BluetoothConnectionService to display
    public void showBluetoothConnected() {
        bluetoothToolBar.setBackgroundColor(Color.parseColor("#2196F3"));
        bluetoothToolBarText.setText("Bluetooth: Connected");
    }

    public void showBluetoothDisconnected() {
        bluetoothToolBar.setBackgroundColor(Color.parseColor("#6C6D6D"));
        bluetoothToolBarText.setText("Bluetooth: Not Connected");
    }

    @RequiresApi(api = Build.VERSION_CODES.M)
    public void btnDiscover() {
        //Log.d(TAG, "btnDiscover: Looking for unpaired devices.");
        Toast.makeText(getApplicationContext(), "Scanning...", Toast.LENGTH_LONG).show();

        if (mBluetoothAdapter.isDiscovering()) {
            mBluetoothAdapter.cancelDiscovery();
            //Log.d(TAG, "btnDiscover: Canceling discovery.");

            //check BT permissions in manifest
            checkBTPermissions();

            mBluetoothAdapter.startDiscovery();
            IntentFilter discoverDevicesIntent = new IntentFilter(BluetoothDevice.ACTION_FOUND);
            registerReceiver(mBroadcastReceiver3, discoverDevicesIntent);
        }
        if (!mBluetoothAdapter.isDiscovering()) {

            //check BT permissions in manifest
            checkBTPermissions();

            mBluetoothAdapter.startDiscovery();
            IntentFilter discoverDevicesIntent = new IntentFilter(BluetoothDevice.ACTION_FOUND);
            registerReceiver(mBroadcastReceiver3, discoverDevicesIntent);
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.M)
    public void btnNEWDiscover() {
        //Log.d(TAG, "btnDiscover: Looking for unpaired devices.");
        Toast.makeText(getApplicationContext(), "Scanning...", Toast.LENGTH_LONG).show();
        if (mBluetoothAdapter.isDiscovering()) {
            mBluetoothAdapter.cancelDiscovery();
            //Log.d(TAG, "btnDiscover: Canceling discovery.");

            //check BT permissions in manifest
            checkBTPermissions();

            mBluetoothAdapter.startDiscovery();
            IntentFilter discoverDevicesIntent = new IntentFilter(BluetoothDevice.ACTION_FOUND);
            registerReceiver(mBroadcastReceiver13, discoverDevicesIntent);
        }
        if (!mBluetoothAdapter.isDiscovering()) {

            //check BT permissions in manifest
            checkBTPermissions();

            mBluetoothAdapter.startDiscovery();
            IntentFilter discoverDevicesIntent = new IntentFilter(BluetoothDevice.ACTION_FOUND);
            registerReceiver(mBroadcastReceiver13, discoverDevicesIntent);
        }
    }

    /**
     * This method is required for all devices running API23+
     * Android must programmatically check the permissions for bluetooth. Putting the proper permissions
     * in the manifest is not enough.
     * <p>
     * NOTE: This will only execute on versions > LOLLIPOP because it is not needed otherwise.
     */
    @RequiresApi(api = Build.VERSION_CODES.M)
    private void checkBTPermissions() {
        if (Build.VERSION.SDK_INT > Build.VERSION_CODES.LOLLIPOP) {
            int permissionCheck = this.checkSelfPermission("Manifest.permission.ACCESS_FINE_LOCATION");
            permissionCheck += this.checkSelfPermission("Manifest.permission.ACCESS_COARSE_LOCATION");
            if (permissionCheck != 0) {

                this.requestPermissions(new String[]{Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.ACCESS_COARSE_LOCATION}, 1001); //Any number
            }
        } else {
            //Log.d(TAG, "checkBTPermissions: No need to check permissions. SDK version < LOLLIPOP.");
        }
    }

    // Broadcast Receiver function
    BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String text = intent.getStringExtra("theMessage");

            //Log.d(TAG, "Text: " + text);
            /* EXPLORE|
            FFC07F80FE01F800E001800300000000000000000000000000000000000000000007000E001F
            |000002000800200080000000000000000000000000000000000000000000000000000000000
            |[2,2]|Left  y,x|direction*/

            //if (text.contains("EX")){
            if (text.substring(0,2).equals("EX")){
                //String statusTag;
                //statusTag = "Robot Status: EXPLORING";

                //((MapTab) pageradapter.fragment1).setIncomingText(statusTag);

                String[] RPiString = new String[4];
                String[] coordinates = new String[1];
                String[] irpic = new String[2]; //ir string

                //Split the string into multiple parts and save to array
                RPiString = text.trim().split("\\|+");

                //Remove square brackets and comma
                coordinates = RPiString[3].replaceAll("\\[", "").replaceAll("\\]", "").trim().split(",");

                //Remove square brackets and comma for ir
                irpic = RPiString[5].replaceAll("\\[", "").replaceAll("\\]", "").trim().split(",");


                ((MapTab) pageradapter.fragment1).setMapExploredObstacles(RPiString[1], RPiString[2]);

                //Row / Column
                ((MapTab) pageradapter.fragment1).setRobotCoordinates(Integer.parseInt(coordinates[0]), Integer.parseInt(coordinates[1]));
                ((MapTab) pageradapter.fragment1).setRobotDirection(RPiString[4]);

                messages.append(text + "\n");
                ((CommsTab) pageradapter.fragment3).setIncomingText(messages);

                mdf_messages.setLength(0);
                mdf_messages.append(RPiString[1]+" | "+RPiString[2]);
                ((CommsTab) pageradapter.fragment3).setMDFText(mdf_messages);

                String irdir = "U";
                String irx = coordinates[0];
                String iry = coordinates[1];

                //String[] irdir = new String[0]; //ir string
                // handle numberid printing

                if (!irpic[0].equals("-1")) {
                    irx = coordinates[0];
                    iry = coordinates[1];
                    if (RPiString[4].equals("U")){
                        irdir = "L"; //ir string
                        iry = Integer.toString(Integer.parseInt(coordinates[1]) + 1);
                        irx = Integer.toString(Integer.parseInt(coordinates[0]) + 2);
                    }
                    else if (RPiString[4].equals("L")){
                        irdir = "D"; //ir string
                        irx = Integer.toString(Integer.parseInt(coordinates[0]) - 1);
                        iry = Integer.toString(Integer.parseInt(coordinates[1]) + 2);
                    }
                    else if (RPiString[4].equals("D")){
                        irdir = "R"; //ir string
                        iry = Integer.toString(Integer.parseInt(coordinates[1]) - 1);
                        irx = Integer.toString(Integer.parseInt(coordinates[0]) - 2);
                    }
                    else if (RPiString[4].equals("R")){
                        irdir = "U"; //ir string
                        irx = Integer.toString(Integer.parseInt(coordinates[0]) + 1);
                        iry = Integer.toString(Integer.parseInt(coordinates[1]) - 2);
                    }
                    if (Integer.parseInt(irx)<15 && Integer.parseInt(iry)<20 && Integer.parseInt(irx)>=0 && Integer.parseInt(iry)>=0) {
                        String newtext;
                        newtext = irx + ',' + iry + ',' + irpic[0] + ',' + irdir;
                        ((MapTab) pageradapter.fragment1).displayNumberID(newtext);

                        nid_messages.append('(' + irpic[0] + ',' + irx + ',' + iry + "), ");
                        ((CommsTab) pageradapter.fragment3).setNIDText(nid_messages);
                    }
                }

                if (!irpic[1].equals("-1")) {
                    irx = coordinates[0];
                    iry = coordinates[1];
                    if (RPiString[4].equals("U")){
                        irdir = "L"; //ir string
                        irx = Integer.toString(Integer.parseInt(coordinates[0]) + 2);
                        iry = coordinates[1];
                    }
                    else if (RPiString[4].equals("L")){
                        irdir = "D"; //ir string
                        iry = Integer.toString(Integer.parseInt(coordinates[1]) + 2);
                        irx = coordinates[0];
                    }
                    else if (RPiString[4].equals("D")){
                        irdir = "R"; //ir string
                        irx = Integer.toString(Integer.parseInt(coordinates[0]) - 2);
                        iry = coordinates[1];
                    }
                    else if (RPiString[4].equals("R")){
                        irdir = "U"; //ir string
                        iry = Integer.toString(Integer.parseInt(coordinates[1]) - 2);
                        irx = coordinates[0];
                    }
                    if (Integer.parseInt(irx)<15 && Integer.parseInt(iry)<20 && Integer.parseInt(irx)>=0 && Integer.parseInt(iry)>=0) {
                        String newtext;
                        newtext = irx + ',' + iry + ',' + irpic[1] + ',' + irdir;
                        ((MapTab) pageradapter.fragment1).displayNumberID(newtext);

                        nid_messages.append('(' + irpic[1] + ',' + irx + ',' + iry + "), ");
                        ((CommsTab) pageradapter.fragment3).setNIDText(nid_messages);
                    }
                }

                if (!irpic[2].equals("-1")) {
                    irx = coordinates[0];
                    iry = coordinates[1];
                    if (RPiString[4].equals("U")){
                        irdir = "L"; //ir string
                        iry = Integer.toString(Integer.parseInt(coordinates[1]) - 1);
                        irx = Integer.toString(Integer.parseInt(coordinates[0]) + 2);
                    }
                    else if (RPiString[4].equals("L")){
                        irdir = "D"; //ir string
                        irx = Integer.toString(Integer.parseInt(coordinates[0]) + 1);
                        iry = Integer.toString(Integer.parseInt(coordinates[1]) + 2);
                    }
                    else if (RPiString[4].equals("D")){
                        irdir = "R"; //ir string
                        iry = Integer.toString(Integer.parseInt(coordinates[1]) + 1);
                        irx = Integer.toString(Integer.parseInt(coordinates[0]) - 2);
                    }
                    else if (RPiString[4].equals("R")){
                        irdir = "U"; //ir string
                        irx = Integer.toString(Integer.parseInt(coordinates[0]) - 1);
                        iry = Integer.toString(Integer.parseInt(coordinates[1]) - 2);
                    }
                    if (Integer.parseInt(irx)<15 && Integer.parseInt(iry)<20 && Integer.parseInt(irx)>=0 && Integer.parseInt(iry)>=0) {
                        String newtext;
                        newtext = irx + ',' + iry + ',' + irpic[2] + ',' + irdir;
                        ((MapTab) pageradapter.fragment1).displayNumberID(newtext);

                        nid_messages.append('(' + irpic[2] + ',' + irx + ',' + iry + "), ");
                        ((CommsTab) pageradapter.fragment3).setNIDText(nid_messages);
                    }
                }

                /*
                if (!irpic[0].equals("-1")) {
                    nid_messages.append('(' + irpic[0] + ',' + coordinates[0] + ',' + coordinates[1] + "), ");
                    ((CommsTab) pageradapter.fragment3).setNIDText(nid_messages);
                }
                if (!irpic[1].equals("-1")) {
                    nid_messages.append('(' + irpic[1] + ',' + coordinates[0] + ',' + coordinates[1] + "), ");
                    ((CommsTab) pageradapter.fragment3).setNIDText(nid_messages);
                }
                if (!irpic[2].equals("-1")) {
                    nid_messages.append('(' + irpic[2] + ',' + coordinates[0] + ',' + coordinates[1] + "), ");
                    ((CommsTab) pageradapter.fragment3).setNIDText(nid_messages);
                }
                */
                /*
                if (text.contains("DONE")) {
                    // display the mdf string
                }
                */

            } /*else if (text.contains("FP")) {
                //Insert Fastest Path code here;
                //FASTEST|flfffffrffffflffrffflfflflffrffffrfffffffffrfffffffflf
                //String statusTag;
                //statusTag = "Robot Status: FASTEST";
                //((MapTab) pageradapter.fragment1).setIncomingText(statusTag);

                String[] RPiString = new String[2];
                String[] coordinates = new String[1];

                //Split the string into multiple parts and save to array
                RPiString = text.trim().split("\\|+");

                //Remove square brackets and comma
                coordinates = RPiString[1].replaceAll("\\[", "").replaceAll("\\]", "").trim().split(",");

                //((MapTab) pageradapter.fragment1).setMapExploredObstacles(RPiString[1], RPiString[2]);

                //Row / Column
                ((MapTab) pageradapter.fragment1).setRobotCoordinates(Integer.parseInt(coordinates[0]), Integer.parseInt(coordinates[1]));
                ((MapTab) pageradapter.fragment1).setRobotDirection(RPiString[2]);

                messages.append(text + "\n");
                ((CommsTab) pageradapter.fragment3).setIncomingText(messages);

                //mdf_messages.setLength(0);
                //mdf_messages.append(RPiString[2]);
                //((CommsTab) pageradapter.fragment3).setMDFText(mdf_messages);

                //String[] RPiString = new String[1];
                //RPiString = text.trim().split("\\|");

                //String[] commands = RPiString[1].trim().split("");

                //  for(int i = 0; i < commands.length; i++){
                //    Log.d("TAG", "Commands: " + commands[i]);
                //}

                //((MapTab) pageradapter.fragment1).runFastestThread(commands);
                //((MapTab) pageradapter.fragment1).setRobotCoordinates(5,5);
            }
            //else if (text.contains("WP")) {
            //    String[] RPiString = new String[1];
            //    String[] coordinates = new String[1];
            //    //Split the string into multiple parts and save to array
            //    RPiString = text.trim().split("\\|+");
            //    //Remove square brackets and comma
            //    coordinates = RPiString[1].replaceAll("\\[", "").replaceAll("\\]", "").trim().split(",");
            //    ((MapTab) pageradapter.fragment1).newwaypoint(Integer.parseInt(coordinates[0]), Integer.parseInt(coordinates[1]));
            //}
            else if (text.contains("NID")) {
                String[] RPiString = new String[3];
                String[] coordinates = new String[1];

                //Split the string into multiple parts and save to array
                RPiString = text.trim().split("\\|+");

                //Remove square brackets and comma
                coordinates = RPiString[1].replaceAll("\\[", "").replaceAll("\\]", "").trim().split(",");

                String newtext;
                newtext = coordinates[0] +',' + coordinates[1] + ','+RPiString[3] +','+RPiString[2];
                ((MapTab) pageradapter.fragment1).displayNumberID(newtext);

                nid_messages.append('('+RPiString[3] +','+coordinates[0] +',' + coordinates[1] +"), ");
                ((CommsTab) pageradapter.fragment3).setNIDText(nid_messages);

                //example {"sendNumberID":("x, y, NumberID, direction")}
                //text = text.replace("\"NID\"", "");
                //Pattern pattern = Pattern.compile("\"(.*?)\"");
                //Matcher matcher = pattern.matcher(text);
                //if (matcher.find()) {
                //    text = matcher.group();
                //    text = text.replace("\"", "");
                //    Log.d(TAG, text);
                //    ((MapTab) pageradapter.fragment1).displayNumberID(text);
                //}

            }*/
            else if (text.contains("STA")) {
                String statusTag;
                statusTag = "Robot Status: "+ text.substring(4);
                ((MapTab) pageradapter.fragment1).setIncomingText(statusTag);
            }
            else {
                messages.append(text + "\n");
                ((CommsTab) pageradapter.fragment3).setIncomingText(messages);
            }
        }
    };

}