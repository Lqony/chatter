package pl.messenger.benq.benqmessenger;

import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.support.v4.content.res.TypedArrayUtils;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.format.Formatter;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

import java.io.IOException;
import java.lang.reflect.Array;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.math.BigInteger;
import java.net.InetAddress;
import java.nio.charset.StandardCharsets;
import java.util.concurrent.ForkJoinPool;

public class ServerActivity1 extends AppCompatActivity {

    private String used_ip_addres = null;
    protected boolean connected = false;

    Runnable InitServer = new Runnable() {
        @Override
        public void run() {
            while (connected == false) {
                CheckConnection();
                //Put some elay like 3 sec
            }
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    ShowIPAndPort();
                }
            });
            LaunchServer(used_ip_addres);
            while(connected){
                ManageConnections();
                if(Received()){
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            ReceiveAndShowData(ReceivedData());
                        }
                    });
                }
            }
        }

        ;
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_server1);

        Thread init_server_thread = new Thread(InitServer);
        init_server_thread.start();
     }


    private void CheckConnection() {
        WifiManager manager = (WifiManager) getApplicationContext().getSystemService(WIFI_SERVICE);
        if (manager.isWifiEnabled()) {
            WifiInfo info = manager.getConnectionInfo();
            if(info.getNetworkId() != -1) {
                InetAddress my_inet_ip = null;
                byte[] ip_address = BigInteger.valueOf(info.getIpAddress()).toByteArray();
                byte l_temp;
                for (int i = 0; i < ip_address.length / 2; i++) {
                    l_temp = ip_address[i];
                    ip_address[i] = ip_address[ip_address.length - i - 1];
                    ip_address[ip_address.length - i - 1] = l_temp;
                }
                try {
                    my_inet_ip = InetAddress.getByAddress(ip_address);
                } catch (IOException e) {
                    //TODO
                }
                if(my_inet_ip != null) {
                    String ip = my_inet_ip.getHostAddress();
                    used_ip_addres = new String();
                    used_ip_addres = ip;
                    connected = true;
                }
            }
        }else{
            boolean ap_state = false;
            try {
                 final Method method = manager.getClass().getDeclaredMethod("isWifiApEnabled");
                method.setAccessible(true);
                ap_state = (boolean) method.invoke(manager);
            }catch (NoSuchMethodException e){
                //TODO
            }catch (IllegalAccessException e){
                //TODO
            }catch (InvocationTargetException e){
                //TODO
            }

            if(ap_state){
                used_ip_addres = new String();
                used_ip_addres = "192.168.43.1";
                connected = true;
            }
        }
    }
    private void ShowIPAndPort(){
        //TODO
        TextView port = (TextView) findViewById(R.id.serverport);
        TextView IP = (TextView) findViewById(R.id.serverip);
        IP.setText("Your IP: " + used_ip_addres);
        port.setText("Port Used: " + Integer.toString(GetPort()));
    }
    private void ReceiveAndShowData(String message){
        TextView board = (TextView) findViewById(R.id.boardtextview);
        String new_message = board.getText().toString() + "\n" + message;
        board.setText(new_message);
        Log.d("RECIEVED: ", message);
    }

    public void SendMessageButton(View view){
        EditText message = (EditText) findViewById(R.id.messageedittext);
        TextView board = (TextView) findViewById(R.id.boardtextview);
        String message_input = message.getText().toString();
        String board_input = board.getText().toString();
        board.setText(board_input + "\n" + message_input);
        message.setText("");

        ServerSendMessage(message_input.toCharArray(), message_input.length());
    }

    public native void LaunchServer(String ip_addres);
    public native int GetPort();
    public native void ManageConnections();
    public native void ServerSendMessage(char [] n_data, int length);
    public native boolean Received();
    public native String ReceivedData();
}
