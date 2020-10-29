package com.example.mdpgroup16;


import android.content.SharedPreferences;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;

import androidx.fragment.app.Fragment;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import java.nio.charset.Charset;

import static android.content.Context.MODE_PRIVATE;
import static com.example.mdpgroup16.MainActivity.SHARED_PREFS;
import static com.example.mdpgroup16.MainActivity.TEXT_F1;
import static com.example.mdpgroup16.MainActivity.TEXT_F2;


/**
 * A simple {@link Fragment} subclass.
 */
public class CommsTab extends Fragment {


    public CommsTab() {
        // Required empty public constructor
    }

    public MainActivity mainActivityObj;
    private TextView incomingMessages, mdfMessage, nidMessage;
    private View view;
    private String oldF1text, oldF2text;
    private String newF1text, newF2text;
    private TextView textViewF1, textViewF2;
    private Button btnF1, btnF2;
    private EditText editTextF1, editTextF2;



    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState){
        // Inflate the layout for this fragment
        if (view == null) {
            // get the main activity from MainActivity class
            mainActivityObj = (MainActivity) getActivity();

            view = inflater.inflate(R.layout.communication, container, false);

            // Send button
            Button btnSend = (Button) view.findViewById(R.id.btnSend);
            final EditText etSend = (EditText) view.findViewById(R.id.editText);

            btnSend.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    if(mainActivityObj.mBluetoothConnection != null) {
                        byte[] bytes = etSend.getText().toString().getBytes(Charset.defaultCharset());
                        mainActivityObj.mBluetoothConnection.write(bytes);

                        etSend.setText("");
                    }
                }
            });


            // Find the view for the reconfigure dialog
            mainActivityObj.myReconfigureDialog.setContentView(R.layout.custompopup);

            // F1 button
            btnF1 = (Button)view.findViewById(R.id.btnF1);

            btnF1.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    // Send message
                    if(mainActivityObj.mBluetoothConnection != null) {
                        byte[] bytes = btnF1.getText().toString().getBytes(Charset.defaultCharset());
                        mainActivityObj.mBluetoothConnection.write(bytes);
                    }
                }
            });

            // F2 button
            btnF2 = (Button)view.findViewById(R.id.btnF2);

            btnF2.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    // Send message
                    if(mainActivityObj.mBluetoothConnection != null) {
                        byte[] bytes = btnF2.getText().toString().getBytes(Charset.defaultCharset());
                        mainActivityObj.mBluetoothConnection.write(bytes);
                    }
                }
            });

            // Reconfigure button
            Button btnReconfigure = (Button) view.findViewById(R.id.btnReconfigure);

            btnReconfigure.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    mainActivityObj.myReconfigureDialog.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));
                    mainActivityObj.myReconfigureDialog.show();
                }
            });

            // F1,F2 edit text and text view
            editTextF1 = (EditText) mainActivityObj.myReconfigureDialog.findViewById(R.id.editTextF1);
            textViewF1 = (TextView) mainActivityObj.myReconfigureDialog.findViewById(R.id.textViewF1Old);
            editTextF2 = (EditText) mainActivityObj.myReconfigureDialog.findViewById(R.id.editTextF2);
            textViewF2 = (TextView) mainActivityObj.myReconfigureDialog.findViewById(R.id.textViewF2Old);

            // Save button
            Button btnSave =(Button) mainActivityObj.myReconfigureDialog.findViewById(R.id.btnSave);

            btnSave.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {

                    // Check if edit text is null
                    newF1text = editTextF1.getText().toString();
                    newF2text = editTextF2.getText().toString();
                    if(newF1text.matches("")){
                        newF1text = oldF1text;
                    }else{
                        editTextF1.setText("");
                    }
                    if(newF2text.matches("")){
                        newF2text = oldF2text;
                    }else{
                        editTextF2.setText("");
                    }
                    saveData(newF1text, newF2text);
                    updateViews(newF1text, newF2text);
                    mainActivityObj.myReconfigureDialog.dismiss();
                }
            });

            // Cancel button
            Button btnCancel =(Button) mainActivityObj.myReconfigureDialog.findViewById(R.id.btnCancel);

            btnCancel.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    mainActivityObj.myReconfigureDialog.dismiss();
                }
            });

            // Receive Messages
            incomingMessages = (TextView) view.findViewById(R.id.incomingMessage);
            mdfMessage = (TextView) view.findViewById(R.id.mdfText);
            nidMessage = (TextView) view.findViewById(R.id.nidText);

            loadData();
            updateViews(oldF1text, oldF2text);
        }
        return view;
    }

    /*
        Set the text for broadcast receiving
     */
    public void setIncomingText(StringBuilder yourText){
        incomingMessages.setText(yourText);
    }
    public void setMDFText(StringBuilder yourText){ mdfMessage.setText(yourText); }
    public void setNIDText(StringBuilder yourText){
        nidMessage.setText(yourText);
    }

    public void saveData(String newTextF1, String newTextF2) {
        SharedPreferences sharedPreferences = mainActivityObj.getApplicationContext().getSharedPreferences(SHARED_PREFS, MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();

        editor.putString(TEXT_F1, newTextF1);
        oldF1text = newTextF1;
        editor.apply();

        editor.putString(TEXT_F2, newTextF2);
        oldF2text = newTextF2;
        editor.apply();

        Toast.makeText(mainActivityObj.getApplicationContext(), "Data saved", Toast.LENGTH_SHORT).show();
    }

    public void loadData() {
        SharedPreferences sharedPreferences = mainActivityObj.getApplicationContext().getSharedPreferences(SHARED_PREFS, MODE_PRIVATE);
        oldF1text = sharedPreferences.getString(TEXT_F1, "pCALIBRATE");
        oldF2text = sharedPreferences.getString(TEXT_F2, "");
    }

    public void updateViews(String textF1, String textF2) {
        btnF1.setText(textF1);
        textViewF1.setText(textF1);
        btnF2.setText(textF2);
        textViewF2.setText(textF2);
    }


}
