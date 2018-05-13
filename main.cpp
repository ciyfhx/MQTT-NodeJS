#include <nan.h>
#include <cstdio>
#include <MQTTClient.h>
#include "CiyMQTT.h"

using namespace ciy;

#define TIMEOUT     10000L

void delivered(void* context, MQTTClient_deliveryToken dt){
	Nan::HandleScope scope;
	v8::Isolate* isolate = v8::Isolate::GetCurrent();
	v8::Local<v8::Value> args[] = { Nan::New(isolate, dt) };

	CiyMQTTClient* client = static_cast<CiyMQTTClient*>(context);

	if(client->deliveredCB != nullptr){
		client->deliveredCB->Call(1, args);
	}

	// if(client->deliveredCB != NULL){
	// 	client->deliveredCB->Call(1, args);
	// }

	// Nan::AsyncQueueWorker(new OnDeliveredCallback(
	// 	client->deliveredCB, dt));

}

int message_arrived(void* context, char* topicName, int topicLen, MQTTClient_message* message){
	// v8::Local<v8::Value> args[]{ Nan::New(topicName).ToLocalChecked(), Nan::New(message->payload) };

	// CiyMQTTClient* client = static_cast<CiyMQTTClient*>(context);

	// if(client->arrivedCB != nullptr){
	// 	Nan::Callback cb(*client->arrivedCB);
	// 	cb.Call(2, args);
	// }

	// MQTTClient_freeMessage(&message);
	// MQTTClient_free(topicName);
	return 1;
}

void lost_connection(void* context, char* cause){
	// v8::Local<v8::Value> args[]{ Nan::New(cause).ToLocalChecked() };

	// CiyMQTTClient* client = static_cast<CiyMQTTClient*>(context);

	// if(client->lostConnCB != nullptr){
	// 	Nan::Callback cb(*client->arrivedCB);
	// 	cb.Call(1, args);
	// }
}	

NAN_METHOD(CiyMQTTClient::OnDelivered) {
	v8::Local<v8::Function> cbFunc = v8::Local<v8::Function>::Cast(info[0]);

	CiyMQTTClient* self = Nan::ObjectWrap::Unwrap<CiyMQTTClient>(info.This());

	self->deliveredCB = new Nan::Callback(cbFunc);

}

NAN_METHOD(CiyMQTTClient::OnArrived) {
	v8::Local<v8::Function> cbFunc = v8::Local<v8::Function>::Cast(info[0]);

	CiyMQTTClient* self = Nan::ObjectWrap::Unwrap<CiyMQTTClient>(info.This());

	self->arrivedCB = new Nan::Callback(cbFunc);

}

NAN_METHOD(CiyMQTTClient::OnConnectionLost) {
	v8::Local<v8::Function> cbFunc = v8::Local<v8::Function>::Cast(info[0]);

	CiyMQTTClient* self = Nan::ObjectWrap::Unwrap<CiyMQTTClient>(info.This());

	self->lostConnCB = new Nan::Callback(cbFunc);

}


//Node JS native binding
NAN_GETTER(CiyMQTTClient::HandleGetters) {
	CiyMQTTClient* self = Nan::ObjectWrap::Unwrap<CiyMQTTClient>(info.This());

	const char* _property = *Nan::Utf8String(property);
	if (_property == "qos") {
		info.GetReturnValue().Set(Nan::New<v8::Int32>(self->qos));
	} else if (_property == "retained") {
		info.GetReturnValue().Set(Nan::New<v8::Int32>(self->retained));
	}else {
		info.GetReturnValue().Set(Nan::Undefined());
  }
}

NAN_SETTER(CiyMQTTClient::HandleSetters) {
	CiyMQTTClient* self = Nan::ObjectWrap::Unwrap<CiyMQTTClient>(info.This());

	if(!value->IsNumber()) {
		return Nan::ThrowError(Nan::New("expected value to be a number").ToLocalChecked());
	}

	const char* _property = *Nan::Utf8String(property);
	if (_property == "qos") {
		self->qos = value->NumberValue();
	} else if (_property == "retained") {
		self->retained = value->NumberValue();
	}
}

NAN_METHOD(CiyMQTTClient::SubscribeTopic) {
	CiyMQTTClient* self = Nan::ObjectWrap::Unwrap<CiyMQTTClient>(info.This());

	Nan::Utf8String _topic(info[0]);

	if (_topic.length() <= 0) {
		Nan::ThrowTypeError("arg must be a non-empty string");
		return;
	}

	MQTTClient_subscribe(self->client, *_topic, 1);
    info.GetReturnValue().SetNull();

}

NAN_METHOD(CiyMQTTClient::UnsubscribeTopic) {
	CiyMQTTClient* self = Nan::ObjectWrap::Unwrap<CiyMQTTClient>(info.This());

	Nan::Utf8String _topic(info[0]);

	if (_topic.length() <= 0) {
		Nan::ThrowTypeError("arg must be a non-empty string");
		return;
	}

	MQTTClient_unsubscribe(self->client, *_topic);
	info.GetReturnValue().SetNull();
}

NAN_METHOD(CiyMQTTClient::PublishTopic) {
	CiyMQTTClient* self = Nan::ObjectWrap::Unwrap<CiyMQTTClient>(info.This());

	Nan::Utf8String _topic(info[0]);
	Nan::Utf8String _payload(info[1]);

	if (_topic.length() <= 0 || _payload.length() <= 0) {
		Nan::ThrowTypeError("arg must be a non-empty string");
		return;
	}

	//return code
	int rc;

	//Craft the mqtt message as well as the token
	MQTTClient_message pubmsg = MQTTClient_message_initializer;
	MQTTClient_deliveryToken token;

	pubmsg.payload = *_payload;
	pubmsg.payloadlen = strlen(*_payload);
	pubmsg.qos = self->qos;
	pubmsg.retained = self->retained;

	MQTTClient_publishMessage(self->client, *_topic, &pubmsg, &token);

	rc = MQTTClient_waitForCompletion(self->client, token, TIMEOUT);


	info.GetReturnValue().Set(Nan::New<v8::Int32>(rc));

}


NAN_METHOD(CiyMQTTClient::Start){
	// unwrap this Client
	CiyMQTTClient* self = Nan::ObjectWrap::Unwrap<CiyMQTTClient>(info.This());

	int rc = 5;

  //  if (_address.length() <= 0 || _client_id.length() <= 0) {
		//Nan::ThrowTypeError("arg must be a non-empty string");
		//return;
  //  }

    MQTTClient_create(&self->client, self->address, self->client_id,
	MQTTCLIENT_PERSISTENCE_NONE, NULL);
	self->conn_opts.keepAliveInterval = 20;
	self->conn_opts.cleansession = 1;

	MQTTClient_setCallbacks(self->client, self, lost_connection, message_arrived, delivered);

	rc = MQTTClient_connect(self->client, &self->conn_opts);

	info.GetReturnValue().Set(Nan::New<v8::Int32>(rc));

}

NAN_METHOD(CiyMQTTClient::Destroy) {
	CiyMQTTClient* self = Nan::ObjectWrap::Unwrap<CiyMQTTClient>(info.This());

	if (!self->client) {
		Nan::ThrowTypeError("Client must be initialize with CreateMQTTClient");
		return;
	}

	MQTTClient_disconnect(self->client, 10000);
	MQTTClient_destroy(&self->client);

	delete self->deliveredCB;
	delete self->arrivedCB;
	delete self->lostConnCB;

	delete self;

	info.GetReturnValue().SetNull();
}

NAN_METHOD(CiyMQTTClient::New) {
	// throw an error if constructor is called without new keyword
	if(!info.IsConstructCall()) {
		Nan::ThrowTypeError("CiyMQTT::New - called without new keyword");
		return;
	}

	// expect exactly 2 arguments
	if(info.Length() != 2) {
		Nan::ThrowTypeError("CiyMQTT::New - expected arguments (host, client_id)");
		return;
	}

	Nan::Utf8String _address(info[0]);//Address
    Nan::Utf8String _client_id(info[1]);//ClientID

	// expect arguments to be numbers
	if(_address.length() <= 0 || _client_id.length() <= 0) {
		Nan::ThrowTypeError("CiyMQTT::New - invalid arguments");
		return;
	}

	// create a new instance and wrap our javascript instance
	CiyMQTTClient* client = new CiyMQTTClient();
	client->Wrap(info.Holder());

	// initialize it's values
	client->address = *_address;
	client->client_id = *_client_id;
	client->qos = 1;
	client->retained = 0;

	// return the wrapped javascript instance
	info.GetReturnValue().Set(info.Holder());
}


// NAN_METHOD(IsPrime) {
//     if (!info[0]->IsNumber()) {
//         Nan::ThrowTypeError("argument must be a number!");
//         return;
//     }
    
//     int number = (int) info[0]->NumberValue();
    
//     if (number < 2) {
//         info.GetReturnValue().Set(Nan::False());
//         return;
//     }
    
//     for (int i = 2; i < number; i++) {
//         if (number % i == 0) {
//             info.GetReturnValue().Set(Nan::False());
//             return;
//         }
//     }
    
//     info.GetReturnValue().Set(Nan::True());
// }

Nan::Persistent<v8::FunctionTemplate> CiyMQTTClient::constructor;

NAN_MODULE_INIT(Initialize) {
    // NAN_EXPORT(target, SubscribeTopic);
	// NAN_EXPORT(target, UnsubscribeTopic);
	// NAN_EXPORT(target, PublishString);
    // NAN_EXPORT(target, CreateMQTTClient);
	// NAN_EXPORT(target, DestroyMQTTClient);

	v8::Local<v8::FunctionTemplate> ctor = Nan::New<v8::FunctionTemplate>(CiyMQTTClient::New);
	CiyMQTTClient::constructor.Reset(ctor);
	ctor->InstanceTemplate()->SetInternalFieldCount(1);
	ctor->SetClassName(Nan::New("CiyMQTT").ToLocalChecked());

	// link our getters and setter to the object property
	Nan::SetAccessor(ctor->InstanceTemplate(), Nan::New("qos").ToLocalChecked(), CiyMQTTClient::HandleGetters, CiyMQTTClient::HandleSetters);
	Nan::SetAccessor(ctor->InstanceTemplate(), Nan::New("retained").ToLocalChecked(), CiyMQTTClient::HandleGetters, CiyMQTTClient::HandleSetters);

	Nan::SetPrototypeMethod(ctor, "start", CiyMQTTClient::Start);
	Nan::SetPrototypeMethod(ctor, "destroy", CiyMQTTClient::Destroy);
	Nan::SetPrototypeMethod(ctor, "publishTopic", CiyMQTTClient::PublishTopic);
	Nan::SetPrototypeMethod(ctor, "subscribeTopic", CiyMQTTClient::SubscribeTopic);
	Nan::SetPrototypeMethod(ctor, "unsubscribeTopic", CiyMQTTClient::UnsubscribeTopic);
	Nan::SetPrototypeMethod(ctor, "onDelivered", CiyMQTTClient::OnDelivered);
	Nan::SetPrototypeMethod(ctor, "onArrived", CiyMQTTClient::OnArrived);
	Nan::SetPrototypeMethod(ctor, "onConnectionLost", CiyMQTTClient::OnConnectionLost);

	target->Set(Nan::New("CiyMQTT").ToLocalChecked(), ctor->GetFunction());

}

NODE_MODULE(mqtt, Initialize);