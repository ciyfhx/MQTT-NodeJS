const { CiyMQTT} = require('./build/Release/mqtt'); // native c++

let client = new CiyMQTT("tcp://localhost:1883", "ExampleClientSub1");


client.onDelivered(function (dt){
    console.log(`Message Delivered: ${dt}`);
});

// client.onArrived(function (topic, message){
//     console.log(`Message Arrived: ${topic}, ${message}`);
// });

const rc = client.start();

console.log(rc)
console.log(client.retained);


if(rc == 0){
    console.log("Connected")
    client.subscribeTopic("MQTT Examples");
    client.publishTopic("MQTT Examples", "Hey");
    while(true){

    }
    
}

client.destroy();


