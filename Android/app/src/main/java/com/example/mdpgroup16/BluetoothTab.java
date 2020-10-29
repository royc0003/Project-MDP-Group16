package com.example.mdpgroup16;


import android.os.Build;
import android.os.Bundle;

import androidx.annotation.RequiresApi;
import androidx.fragment.app.Fragment;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.Toast;


/**
 * A simple {@link Fragment} subclass.
 */
public class BluetoothTab extends Fragment {


    public BluetoothTab() {
        // Required empty public constructor
    }

    // Variables
    public MainActivity mainActivityObj;
    private View view;
    ImageView imgRaspberry;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

        if (view == null) {
            // get the main activity from MainActivity class
            mainActivityObj = (MainActivity) getActivity();

            // Inflate the layout for this fragment
            view = inflater.inflate(R.layout.bluetooth, container, false);

            Button btnDiscover = (Button) view.findViewById(R.id.btnFindUnpairedDevices);
            Button btnNEWDiscover = (Button) view.findViewById(R.id.btnGet);
             imgRaspberry = (ImageView) view.findViewById(R.id.imgRaspberry);

            mainActivityObj.lvNewDevices = (ListView) view.findViewById(R.id.lvNewDevices);
            mainActivityObj.lvNewDevices.setDivider(null);
            mainActivityObj.lvNewDevices.setDividerHeight(0);
            mainActivityObj.lstvw = (ListView) view.findViewById(R.id.deviceList);
            mainActivityObj.lstvw.setDivider(null);
            mainActivityObj.lstvw.setDividerHeight(0);

            mainActivityObj.lvNewDevices.setOnItemClickListener(new AdapterView.OnItemClickListener() {
                @Override
                public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                    //clear current list first

                    //first cancel discovery because its very memory intensive.
                    mainActivityObj.mBluetoothAdapter.cancelDiscovery();

                    //Log.d(TAG, "onItemClick: You Clicked on a device.");
                    String deviceName = mainActivityObj.mBTDevices.get(position).getName();
                    String deviceAddress = mainActivityObj.mBTDevices.get(position).getAddress();

                    //Log.d(TAG, "onItemClick: deviceName = " + deviceName);
                    //Log.d(TAG, "onItemClick: deviceAddress = " + deviceAddress);
                    Toast.makeText(mainActivityObj.getApplicationContext(), "Connecting to " + deviceName, Toast.LENGTH_LONG).show();

                    //create the bond.
                    //NOTE: Requires API 17+? I think this is JellyBean
                    if (Build.VERSION.SDK_INT > Build.VERSION_CODES.JELLY_BEAN_MR2) {
                        //Log.d(TAG, "Trying to pair with " + deviceName);
                        mainActivityObj.mBTDevices.get(position).createBond();

                        mainActivityObj.mBTDevice = mainActivityObj.mBTDevices.get(position);
                        mainActivityObj.mBluetoothConnection = new BluetoothConnectionService(mainActivityObj);
                    }

                    // Connecting to device
                    mainActivityObj.startConnection();
                }
            });
            mainActivityObj.lstvw.setOnItemClickListener(new AdapterView.OnItemClickListener() {
                @Override
                public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                    //clear current list first

                    //first cancel discovery because its very memory intensive.
                    mainActivityObj.mBluetoothAdapter.cancelDiscovery();

                    //Log.d(TAG, "onItemClick: You Clicked on a device.");
                    String deviceName = mainActivityObj.mBTNEWDevices.get(position).getName();
                    String deviceAddress = mainActivityObj.mBTNEWDevices.get(position).getAddress();

                    //Log.d(TAG, "onItemClick: deviceName = " + deviceName);
                    //Log.d(TAG, "onItemClick: deviceAddress = " + deviceAddress);
                    Toast.makeText(mainActivityObj.getApplicationContext(), "Connecting to " + deviceName, Toast.LENGTH_LONG).show();

                    //create the bond.
                    //NOTE: Requires API 17+? I think this is JellyBean
                    if (Build.VERSION.SDK_INT > Build.VERSION_CODES.JELLY_BEAN_MR2) {
                        //Log.d(TAG, "Trying to pair with " + deviceName);
                        mainActivityObj.mBTNEWDevices.get(position).createBond();

                        mainActivityObj.mBTDevice = mainActivityObj.mBTNEWDevices.get(position);
                        mainActivityObj.mBluetoothConnection = new BluetoothConnectionService(mainActivityObj);
                    }

                    // Connecting to device
                    mainActivityObj.startConnection();
                }
            });
            btnNEWDiscover.setOnClickListener(new View.OnClickListener() {
                                               @RequiresApi(api = Build.VERSION_CODES.M)
                                               @Override
                                               public void onClick(View view) {
                                                   //Hide raspberry image
                                                   imgRaspberry.setVisibility(view.INVISIBLE);
                                                   //Log.d(TAG, "onClick: enabling/disabling bluetooth.");
                                                   mainActivityObj.btnNEWDiscover();
                                                   //Clear all list items
                                                   mainActivityObj.mBTNEWDevices.clear();
                                               }
                                           });
            btnDiscover.setOnClickListener(new View.OnClickListener() {
                @RequiresApi(api = Build.VERSION_CODES.M)
                @Override
                public void onClick(View view) {
                    //Hide raspberry image
                    imgRaspberry.setVisibility(view.INVISIBLE);
                    //Log.d(TAG, "onClick: enabling/disabling bluetooth.");
                    mainActivityObj.btnDiscover();
                    //Clear all list items
                    mainActivityObj.mBTDevices.clear();
                }
            });

        }
        return view;
    }

}
