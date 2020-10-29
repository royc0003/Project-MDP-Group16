package com.example.mdpgroup16;

import android.hardware.SensorEvent;
import android.os.Bundle;

import androidx.fragment.app.Fragment;

import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ToggleButton;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.CompoundButton;
import android.util.Log;
import android.os.Handler;

import java.nio.charset.Charset;


/**
 * A simple {@link Fragment} subclass.
 */
public class MapTab extends Fragment {
    ImageButton upBtn;
    ImageButton leftBtn;
    ImageButton downBtn;
    ImageButton rightBtn;
    ImageButton updateBtn;
    ToggleButton waypointBtn;
    ToggleButton tiltBtn;
    Button startBtn;
    Button exploreBtn;
    Button resetBtn;
    MapView mapView;
    Switch autoManualSwitch;
    Button caliFrontBtn;
    Button caliFront2Btn;
    Button caliRightBtn;

    private static final String TAG = "Tab1";
    private int Fastest_Row = 1, Fastest_Col = 1, Fastest_Count = 0;
    private String Fastest_Direction = "Right";
    private String[] Fastest_Commands = new String[150];
    public MainActivity mainActivityObj;
    private View view;
    private TextView statusMessages;
    Handler handler = new Handler();

    public MapTab() {
        // Required empty public constructor
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        if (view == null) {

            // Inflate the layout for this fragment
            mainActivityObj = (MainActivity) getActivity();

            view = inflater.inflate(R.layout.map, container, false);

            mapView = (MapView) view.findViewById(R.id.idMapView);

            upBtn = (ImageButton) view.findViewById(R.id.btnTop);
            leftBtn = (ImageButton) view.findViewById(R.id.btnLeft);
            rightBtn = (ImageButton) view.findViewById(R.id.btnRight);
            downBtn = (ImageButton) view.findViewById(R.id.btnBottom);
            updateBtn = (ImageButton) view.findViewById(R.id.btnUpdateMap);
            waypointBtn = (ToggleButton) view.findViewById(R.id.waypointbtn);
            tiltBtn = (ToggleButton) view.findViewById(R.id.tiltbtn);
            startBtn = (Button) view.findViewById(R.id.startbtn);
            exploreBtn = (Button) view.findViewById(R.id.explorebtn);
            resetBtn = (Button) view.findViewById(R.id.resetbtn);
            autoManualSwitch = (Switch) view.findViewById(R.id.autoSwitch);
            autoManualSwitch.setChecked(true);
            caliFrontBtn = (Button) view.findViewById(R.id.calFront);
            caliFront2Btn = (Button) view.findViewById(R.id.calFront2);
            caliRightBtn = (Button) view.findViewById(R.id.calRight);


            // status Messages
            statusMessages = (TextView) view.findViewById(R.id.txtRobotStatus);

            upBtn.setOnClickListener(new View.OnClickListener() {
                public void onClick(View v) {
                    if (mainActivityObj.mBluetoothConnection != null) {
                        //pMANUAL|f
                        //Log.d(TAG, "hf\n");
                        byte[] bytes = ("0|1|F").getBytes(Charset.defaultCharset());
                        mainActivityObj.mBluetoothConnection.write(bytes);
                    } else {
                        Toast.makeText(getContext(), "Bluetooth not connected!", Toast.LENGTH_SHORT).show();
                    }
                }
            });

            leftBtn.setOnClickListener(new View.OnClickListener() {
                public void onClick(View v) {
                    if (mainActivityObj.mBluetoothConnection != null) {
                        //"pMANUAL|l
                        byte[] bytes = ("0|1|L").getBytes(Charset.defaultCharset());
                        mainActivityObj.mBluetoothConnection.write(bytes);
                    } else {
                        Toast.makeText(getContext(), "Bluetooth not connected!", Toast.LENGTH_SHORT).show();
                    }
                }
            });

            rightBtn.setOnClickListener(new View.OnClickListener() {
                public void onClick(View v) {
                    if (mainActivityObj.mBluetoothConnection != null) {
                        //pMANUAL|r
                        byte[] bytes = ("0|1|R").getBytes(Charset.defaultCharset());
                        mainActivityObj.mBluetoothConnection.write(bytes);
                    } else {
                        Toast.makeText(getContext(), "Bluetooth not connected!", Toast.LENGTH_SHORT).show();
                    }
                }
            });

            downBtn.setOnClickListener(new View.OnClickListener() {
                public void onClick(View v) {
                    if (mainActivityObj.mBluetoothConnection != null) {
                        //pMANUAL|b
                        byte[] bytes = ("0|1|B").getBytes(Charset.defaultCharset());
                        mainActivityObj.mBluetoothConnection.write(bytes);
                    } else {
                        Toast.makeText(getContext(), "Bluetooth not connected!", Toast.LENGTH_SHORT).show();
                    }
                }
            });

            //temp
            caliFrontBtn.setOnClickListener(new View.OnClickListener() {
                public void onClick(View v) {
                    if (mainActivityObj.mBluetoothConnection != null) {
                        byte[] bytes = ("0|1|C").getBytes(Charset.defaultCharset());
                        mainActivityObj.mBluetoothConnection.write(bytes);
                    } else {
                        Toast.makeText(getContext(), "Bluetooth not connected!", Toast.LENGTH_SHORT).show();
                    }
                }
            });

            caliFront2Btn.setOnClickListener(new View.OnClickListener() {
                public void onClick(View v) {
                    if (mainActivityObj.mBluetoothConnection != null) {
                        byte[] bytes = ("0|1|X").getBytes(Charset.defaultCharset());
                        mainActivityObj.mBluetoothConnection.write(bytes);
                    } else {
                        Toast.makeText(getContext(), "Bluetooth not connected!", Toast.LENGTH_SHORT).show();
                    }
                }
            });

            caliRightBtn.setOnClickListener(new View.OnClickListener() {
                public void onClick(View v) {
                    if (mainActivityObj.mBluetoothConnection != null) {
                        byte[] bytes = ("0|1|Y").getBytes(Charset.defaultCharset());
                        mainActivityObj.mBluetoothConnection.write(bytes);
                    } else {
                        Toast.makeText(getContext(), "Bluetooth not connected!", Toast.LENGTH_SHORT).show();
                    }
                }
            });

            startBtn.setOnClickListener(new View.OnClickListener() {
                public void onClick(View v) {
                    if (mainActivityObj.mBluetoothConnection != null) {
                        byte[] bytes = ("0|2|FP_START").getBytes(Charset.defaultCharset());
                        mainActivityObj.mBluetoothConnection.write(bytes);
                        setIncomingText("Robot Status: FASTEST");
                    } else {
                        Toast.makeText(getContext(), "Bluetooth not connected!", Toast.LENGTH_SHORT).show();
                    }
                }
            });

            exploreBtn.setOnClickListener(new View.OnClickListener() {
                public void onClick(View v) {
                    if (mainActivityObj.mBluetoothConnection != null) {
                        byte[] bytes = ("0|2|EX_START").getBytes(Charset.defaultCharset());
                        mainActivityObj.mBluetoothConnection.write(bytes);
                        setIncomingText("Robot Status: EXPLORING");
                    } else {
                        Toast.makeText(getContext(), "Bluetooth not connected!", Toast.LENGTH_SHORT).show();
                    }
                }
            });

            resetBtn.setOnClickListener(new View.OnClickListener() {
                public void onClick(View v) {
                    if (mainActivityObj.mBluetoothConnection != null) {
                        //byte[] bytes = ("reset").getBytes(Charset.defaultCharset());
                        //mainActivityObj.mBluetoothConnection.write(bytes);
                    }
                    mapView.resetMap();
                    Toast.makeText(getContext(), "Map Reset!", Toast.LENGTH_SHORT).show();
                }
            });

            autoManualSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
                public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                    if (isChecked) {
                        if (mainActivityObj.mBluetoothConnection != null) {
                            thread.start();
                        }
                        autoManualSwitch.setTextSize(14);
                        autoManualSwitch.setText("Auto");
                        updateBtn.setVisibility(View.GONE);


                    } else {
                        if (mainActivityObj.mBluetoothConnection != null) {
                            thread.interrupt();
                        }
                        autoManualSwitch.setTextSize(12);
                        autoManualSwitch.setText("Manual");
                        updateBtn.setVisibility(View.VISIBLE);
                    }
                }
            });

            updateBtn.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (mainActivityObj.mBluetoothConnection != null) {
                        //Call the function once to fetch
                        fetchMapCoordinates();
                        Toast.makeText(getContext(), "Fetching Map Info!", Toast.LENGTH_SHORT).show();
                    } else {
                        Toast.makeText(getContext(), "Bluetooth not connected!", Toast.LENGTH_SHORT).show();
                    }
                }
            });

            waypointBtn.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (waypointBtn.isChecked()) {
                        MainActivity.wayPointChecked = true;
                    } else {
                        MainActivity.wayPointChecked = false;
                    }
                    Toast.makeText(getContext(), "Waypoint button pressed!", Toast.LENGTH_SHORT).show();
                }
            });

            tiltBtn.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (tiltBtn.isChecked()) {
                        MainActivity.tiltChecked = true;
                        if (mainActivityObj.mBluetoothConnection != null) {
                            thread.start();
                        }
                    } else {
                        MainActivity.tiltChecked = false;
                        if (mainActivityObj.mBluetoothConnection != null) {
                            thread.interrupt();
                        }
                    }
                    Toast.makeText(getContext(), "Tilt button pressed!", Toast.LENGTH_SHORT).show();
                }
            });

            //noinspection AndroidLintClickableViewAccessibility
            mapView.setOnTouchListener(new View.OnTouchListener() {
                @Override
                public boolean onTouch(View v, MotionEvent event) {
                    float x = event.getX();
                    float y = event.getY();
                    int[] info = mapView.setWaypointOrRobot(x, y);
                    int col = info[0];
                    //Flip the row
                    int row = 19 - info[1];
                    int isWaypoint = info[2];
                    if (isWaypoint == 1) {
                        sendWaypointCoordinates(col, row);

                    }
                    //Disable cos no need to set robot position
                    //To enable it, go to mapview also
                    //else {
                    //    sendRobotCoordinates(col, row);
                    //}
                    return false;
                }
            });
        }

        return view;
    }

    public void setIncomingText(String yourText) {
        statusMessages.setText(yourText);
    }
    public float xval;
    public void setIncomingX(float xvalue) {
        xval = xvalue;
    }
    public float yval;
    public void setIncomingY(float yvalue) {
        yval = yvalue;
    }

    public void newwaypoint(int x,int y){
        y = Math.abs(y - 19);
        mapView.checkwp(x, y);
    }

    public void tilting(){
        if (mainActivityObj.mBluetoothConnection != null) {
            String robotMessage = "";
            if (Math.abs(xval) > Math.abs(yval)) {
                if (Math.abs(xval) > 3) {
                    if (xval > 0) {
                        robotMessage = "0|1|L";
                    } else {
                        robotMessage = "0|1|R";
                    }
                }
            } else {
                if (Math.abs(yval) > 3) {
                    if (yval > 0) {
                        robotMessage = "0|1|B";
                    } else {
                        robotMessage = "0|1|F";
                    }
                }
            }
            byte[] bytes = robotMessage.getBytes(Charset.defaultCharset());
            mainActivityObj.mBluetoothConnection.write(bytes);
        }
        else{
            Toast.makeText(getContext(), "Bluetooth not connected!", Toast.LENGTH_SHORT).show();
        }
    }

    public void setMapExploredObstacles(String exploredHex, String obstaclesHex) {
        mapView.setMapExploredObstacles(exploredHex, obstaclesHex);
    }

    public void setRobotCoordinates(int col, int row) {
        mapView.setRobotCoordinates(col, row);
    }

    public void setRobotDirection(String direction) {
        mapView.setRobotDirection(direction);
    }

    public void sendWaypointCoordinates(int col, int row) {
        if (mainActivityObj.mBluetoothConnection != null) {
            //String waypointMessage = "p|WAYPOINT|" + col + "|" + row;
            String waypointMessage = "0|2|WP;" + col + ';' + row;
            byte[] bytes = waypointMessage.getBytes(Charset.defaultCharset());
            mainActivityObj.mBluetoothConnection.write(bytes);
        }
    }

    public void sendRobotCoordinates(int col, int row) {
        if (mainActivityObj.mBluetoothConnection != null) {
            String robotMessage = "Robot at (" + col + "," + row + ")";
            byte[] bytes = robotMessage.getBytes(Charset.defaultCharset());
            mainActivityObj.mBluetoothConnection.write(bytes);
        }
    }

    public void fetchMapCoordinates() {
        // Send the string "sendArena" to AMDTool
        if (mainActivityObj.mBluetoothConnection != null) {
            byte[] bytes = ("sendArena").getBytes(Charset.defaultCharset());
            mainActivityObj.mBluetoothConnection.write(bytes);
        }
    }

    public void displayNumberID(String numberIDString) {
        // 1. x 2. y 3. numberID 4. direction
        numberIDString = numberIDString.trim();
        String[] numberArr = numberIDString.split(",");
        mapView.initNumberID(numberArr);
    }

    public void runFastestThread(String[] commands) {
        Fastest_Col = 1;
        Fastest_Row = 1;
        Fastest_Count = 1;
        Fastest_Direction = "Right";
        Fastest_Commands = commands;
        handler.post(runnable);
    }

    final Runnable runnable = new Runnable() {
        public void run() {
            if (Fastest_Count++ < Fastest_Commands.length - 1) {
                //FASTEST|rflf
                Log.d("TAG", "Count: " + Fastest_Count);

                if (Fastest_Commands[Fastest_Count].charAt(0) == 'f') {
                    if (Fastest_Direction.contains("Top")) {
                        Log.d(TAG, "Test");
                        Fastest_Row = Fastest_Row + 1;
                        setRobotCoordinates(Fastest_Col, Fastest_Row);
                    } else if (Fastest_Direction.contains("Left")) {
                        Log.d(TAG, "Test1");
                        Fastest_Col = Fastest_Col - 1;
                        setRobotCoordinates(Fastest_Col, Fastest_Row);
                    } else if (Fastest_Direction.contains("Right")) {
                        Log.d(TAG, "Test2");
                        Fastest_Col = Fastest_Col + 1;
                        setRobotCoordinates(Fastest_Col, Fastest_Row);
                    } else {
                    }
                } else if (Fastest_Commands[Fastest_Count].charAt(0) == 'l') {
                    if (Fastest_Direction.contains("Top")) {
                        Fastest_Direction = "Left";
                        setRobotDirection("Left");
                    } else if (Fastest_Direction.contains("Right")) {
                        Fastest_Direction = "Top";
                        setRobotDirection("Top");
                    } else if (Fastest_Direction.contains("Left")) {
                        Fastest_Direction = "Down";
                        setRobotDirection("Down");
                    } else {
                        // if(Fastest_Direction.contains("Down"))
                        Fastest_Direction = "Right";
                        setRobotDirection("Right");
                    }
                } else if (Fastest_Commands[Fastest_Count].charAt(0) == 'r') {
                    if (Fastest_Direction.contains("Top")) {
                        Fastest_Direction = "Right";
                        setRobotDirection("Right");
                    } else if (Fastest_Direction.contains("Right")) {
                        Fastest_Direction = "Down";
                        setRobotDirection("Down");
                    } else if (Fastest_Direction.contains("Left")) {
                        Fastest_Direction = "Top";
                        setRobotDirection("Top");
                    } else {
                        // if(Fastest_Direction.contains("Down"))
                        Fastest_Direction = "Left";
                        setRobotDirection("Left");
                    }
                } else {
                    //Do Nothing
                }
                handler.postDelayed(this, 500);
            }
        }
    };

    Thread thread = new Thread() {
        @Override
        public void run() {
            while (!Thread.currentThread().isInterrupted()) {
                try {
                    Thread.sleep(1000);
                    mainActivityObj.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            //fetchMapCoordinates();
                            tilting();
                            //Log.d(TAG, "Worked!");
                        }
                    });
                } catch (InterruptedException e) {
                    //End the loop on interruption
                    break;
                }
            }
        }
    };
}
