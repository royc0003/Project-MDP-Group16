package utils;

import java.io.*;
import java.net.Socket;
import java.net.UnknownHostException;

/**
 * Communication manager to communicate with the different parts of the system via the RasPi.
 *
 */

public class CommMgr {

    public static final String EX_START = "EX_START";       // Android --> PC
    public static final String FP_START = "FP_START";       // Android --> PC
    public static final String TO_ANDROID = "TO_ANDROID";   // PC --> Android
    public static final String BOT_START = "S";             // PC --> Arduino
    public static final String INSTRUCTIONS = "INSTR";      // PC --> Arduino
    public static final String CALIBRATE = "C";             // PC --> Arduino
    public static final String SENSOR_DATA = "SDATA";       // Arduino --> PC
    public static final String CAMERA = "NID";              // PC --> Rpi
    public static final String DONE_EX = "DONE_EX";         // PC --> Anroid
    public static final String FP_READY = "FP_READY";       // PC --> Android

    private static CommMgr commMgr = null;
    private static Socket conn = null;

    private BufferedWriter writer;
    private BufferedReader reader;

    private CommMgr() {
    }

    public static CommMgr getCommMgr() {
        if (commMgr == null) {
            commMgr = new CommMgr();
        }
        return commMgr;
    }

    public void openConnection() {
        System.out.println("Opening connection...");

        try {
            String HOST = "192.168.16.1";
            // String HOST = "192.168.16.2";
            int PORT = 5560;
            conn = new Socket(HOST, PORT);

            writer = new BufferedWriter(new OutputStreamWriter(new BufferedOutputStream(conn.getOutputStream())));
            reader = new BufferedReader(new InputStreamReader(conn.getInputStream()));

            System.out.println("openConnection() --> " + "Connection established successfully!");

            return;
        } catch (UnknownHostException e) {
            System.out.println("openConnection() --> UnknownHostException");
        } catch (IOException e) {
            System.out.println("openConnection() --> IOException");
        } catch (Exception e) {
            System.out.println("openConnection() --> Exception");
            System.out.println(e.toString());
        }

        System.out.println("Failed to establish connection!");
    }

    public void closeConnection() {
        System.out.println("Closing connection...");

        try {
            reader.close();

            if (conn != null) {
                conn.close();
                conn = null;
            }
            System.out.println("Connection closed!");
        } catch (IOException e) {
            System.out.println("closeConnection() --> IOException");
        } catch (NullPointerException e) {
            System.out.println("closeConnection() --> NullPointerException");
        } catch (Exception e) {
            System.out.println("closeConnection() --> Exception");
            System.out.println(e.toString());
        }
    }

    public void sendMsg(String msg, String msgType) {
        System.out.println("Sending a message...");

        try {
            String outputMsg;
            if (msg == null) {
                outputMsg = "2|1|" + msgType + "\n";
            }
            else if (msgType.equals(TO_ANDROID)){
                outputMsg = "2|0|" + msg;
            } 
            else if (msgType.equals(DONE_EX) || msgType.equals(FP_READY)){
                outputMsg = "2|0|STA|" + msg + "\n";   
            }
            else if(msgType.equals(INSTRUCTIONS)) {
                outputMsg = "2|1|" + msg + "\n";
            }
            else if (msgType.equals(CAMERA)){
                    outputMsg = msg + "\n";
            }
            else {
                outputMsg = msgType + "\n" + msg + "\n";
            }

            System.out.println("Sending out message:\n" + outputMsg);
            writer.write(outputMsg);
            writer.flush();
        } catch (IOException e) {
            System.out.println("sendMsg() --> IOException");
        } catch (Exception e) {
            System.out.println("sendMsg() --> Exception");
            System.out.println(e.toString());
        }
    }

    public String recvMsg() {
        System.out.println("Receiving a message...");

        try {
            StringBuilder sb = new StringBuilder();
            String input = reader.readLine();

            if (input != null && input.length() > 0) {
                sb.append(input);
                System.out.println("Hello received: " + sb.toString()+ "\n");
                return sb.toString();
            }
        } catch (IOException e) {
            System.out.println("recvMsg() --> IOException");
        } catch (Exception e) {
            System.out.println("recvMsg() --> Exception");
            System.out.println(e.toString());
        }

        return null;
    }

    public boolean isConnected() {
        return conn.isConnected();
    }
}
