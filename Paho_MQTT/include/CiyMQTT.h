

#ifndef CiyMQTT
#define CiyMQTT

#include <MQTTClient.h>
#include <nan.h>
#include <type_traits>

namespace ciy{
    class CiyMQTTClient : public Nan::ObjectWrap {

        public:
            MQTTClient client;
            MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

            char* address;
            char* client_id;

            int qos;
            int retained;

            Nan::Callback* deliveredCB;
			Nan::Callback* arrivedCB;
			Nan::Callback* lostConnCB;

            //Constructor New
            static NAN_METHOD(New);

            static NAN_METHOD(Start);
            static NAN_METHOD(Destroy);
            static NAN_METHOD(SubscribeTopic);
            static NAN_METHOD(UnsubscribeTopic);
            static NAN_METHOD(PublishTopic);

            static NAN_METHOD(OnDelivered);
            static NAN_METHOD(OnArrived);
            static NAN_METHOD(OnConnectionLost);

            static NAN_GETTER(HandleGetters);
            static NAN_SETTER(HandleSetters);

			static Nan::Persistent<v8::FunctionTemplate> constructor;


	};

    template<typename T>
    class AsyncCallback : public Nan::AsyncWorker {
        public:

            AsyncCallback(Nan::Callback *callback)
                : Nan::AsyncWorker(callback) {

            }

            void Forward() {
                static_cast<T*>(this)->Forward();
            }

            void Execute() {
                //Do nothing we just want to relay the callback
            }

            void HandleOKCallback() {
                //Prevents Garbage Collection of v8 Objects immediately after leaving function
                Nan::HandleScope scope;
                Forward();
                // v8::Local<v8::Value> argv[] = {
                // Nan::Null(), // no error occured
                // Nan::New(workerId).ToLocalChecked()
                // };
                // callback->Call(2, argv);
            }

            void HandleErrorCallback() {
                Nan::HandleScope scope;
                Forward();
                // v8::Local<v8::Value> argv[] = {
                // Nan::New(this->ErrorMessage()).ToLocalChecked(), // return error message
                // Nan::Null()
                // };
                // callback->Call(2, argv);
            }
    };

    class OnDeliveredCallback : public AsyncCallback<OnDeliveredCallback> {
        public:
            MQTTClient_deliveryToken dt; 
			OnDeliveredCallback(Nan::Callback* callback, MQTTClient_deliveryToken dt) : AsyncCallback(callback), dt(dt){

            }

            void Forward() {
                v8::Local<v8::Value> args[] = { Nan::New(dt) };
                callback->Call(1, args);
            }
    };



}


#endif