#include <jni.h>
#include <string>
#include "ServerClass.h"
#include "ClientClass.h"

ServerClass *MainServer;
ClientClass *MyClient;

extern "C"
JNIEXPORT jstring JNICALL
Java_pl_messenger_benq_benqmessenger_ServerActivity1_ReceivedData(JNIEnv *env, jobject instance) {

    string returnValue = MainServer->ReceivedData();
    return env->NewStringUTF(returnValue.c_str());
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_pl_messenger_benq_benqmessenger_ServerActivity1_Received(JNIEnv *env, jobject instance) {

    return MainServer->Received();

}

extern "C"
JNIEXPORT void JNICALL
Java_pl_messenger_benq_benqmessenger_ClientActivity1_SendData(JNIEnv *env, jobject instance,
                                                              jstring message_, jint length) {
    const char *message = env->GetStringUTFChars(message_, 0);

    MyClient->SendMessage((char*)message, length);

    env->ReleaseStringUTFChars(message_, message);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_pl_messenger_benq_benqmessenger_ClientActivity1_GetMessage(JNIEnv *env, jobject instance) {

    // TODO
    string returnValue = MyClient->GetMessage();
    return env->NewStringUTF(returnValue.c_str());
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_pl_messenger_benq_benqmessenger_ClientActivity1_ReceiveData(JNIEnv *env, jobject instance) {

    return MyClient->Receive();

}

extern "C"
JNIEXPORT void JNICALL
Java_pl_messenger_benq_benqmessenger_ServerActivity1_ServerSendMessage(JNIEnv *env,
                                                                       jobject instance,
                                                                       jcharArray n_data_,
                                                                       jint length) {
    jchar *n_data = env->GetCharArrayElements(n_data_, NULL);
    char * l_data = new char[length];
    for(int i = 0; i < length; i++){
        l_data[i] = n_data[i];
    }
    MainServer->SendMessage(l_data, length);
    // TODO

    env->ReleaseCharArrayElements(n_data_, n_data, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_pl_messenger_benq_benqmessenger_ClientActivity1_InitClient(JNIEnv *env, jobject instance) {

    // TODO
    MyClient = new ClientClass;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_pl_messenger_benq_benqmessenger_ClientActivity1_NativeConnectToServer(JNIEnv *env,
                                                                           jobject instance,
                                                                           jint n_port,
                                                                           jstring n_ip_address_) {
    bool result;
    const char *n_ip_address = env->GetStringUTFChars(n_ip_address_, 0);

    result = MyClient->ConnectToServer(n_port, (char*)n_ip_address);

    env->ReleaseStringUTFChars(n_ip_address_, n_ip_address);

    return result;
}

extern "C"
JNIEXPORT void JNICALL
Java_pl_messenger_benq_benqmessenger_ServerActivity1_ManageConnections(JNIEnv *env,
                                                                        jobject instance) {

    MainServer->ManageConnections();

}

extern "C"
JNIEXPORT jint JNICALL
Java_pl_messenger_benq_benqmessenger_ServerActivity1_GetPort(JNIEnv *env, jobject instance) {
    // TODO
    return MainServer->Port();
}

extern "C"
JNIEXPORT void JNICALL
Java_pl_messenger_benq_benqmessenger_ServerActivity1_LaunchServer(JNIEnv *env, jobject instance,
                                                                  jstring ip_addres_) {
    const char *ip_addres = env->GetStringUTFChars(ip_addres_, 0);

    // TODO
    MainServer = new ServerClass((char*)ip_addres);
    env->ReleaseStringUTFChars(ip_addres_, ip_addres);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_pl_messenger_benq_benqmessenger_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
