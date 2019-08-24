package pl.messenger.benq.benqmessenger;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

public class ClientActivity1 extends AppCompatActivity {

    String server_ip_address = null;
    Integer server_port = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_client1);
        InitClient();
    }

    public void ConnectToServer(View view){
        EditText ip_address = (EditText) findViewById(R.id.iptext);
        EditText port = (EditText) findViewById(R.id.porttext);
        TextView conn_state = (TextView) findViewById(R.id.connstatetext);

        server_ip_address = ip_address.getText().toString();
        if(port.getText().toString().isEmpty() == false)
        server_port = Integer.parseInt(port.getText().toString());

        if(!server_ip_address.isEmpty() && !port.getText().toString().isEmpty()){

            if(NativeConnectToServer(server_port, server_ip_address)){
                conn_state.setText("Connected to: " + server_ip_address + " ; " + server_port);
                Thread ClientReceiveData = new Thread(RecieveData);
                ClientReceiveData.start();
            }else{
                conn_state.setText("Could not connect to: " + server_ip_address + " ; " + server_port);
            }
        }
    }
    public void SendMessage(View view){
        EditText message = (EditText)findViewById(R.id.messageedittext);
        TextView board = (TextView) findViewById(R.id.boardtextview);
        String input_message = board.getText().toString() + "\n" + message.getText().toString();
        board.setText(input_message);
        input_message = message.getText().toString();
        message.setText("");
        SendData(input_message, input_message.length());
    }

    Runnable RecieveData = new Runnable() {
        @Override
        public void run() {
            while(true) {
                if (ReceiveData()) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            ReceiveMessage(GetMessage());
                        }
                    });
                }
            }
        }
    };

    private void ReceiveMessage(String message){
        String new_message;
        TextView board = (TextView) findViewById(R.id.boardtextview);
        new_message = board.getText() + "\n" + message;
        board.setText(new_message);
    }


    public native boolean NativeConnectToServer(int n_port, String n_ip_address);
    public native void InitClient();
    private native boolean ReceiveData();
    private native String GetMessage();
    public native void SendData(String message, int length);
}
