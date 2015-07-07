/*jslint node: true */
"use strict";
Buffer.prototype.toByteArray = function () {
    return Array.prototype.slice.call(this, 0);
};


/* | LightBlue Bean to OSC via NOBLE.
 ---|---------------------------------*/

var scratchOne = "a495ff21c5b14b44b5121370f02d74de",
    scratchTwo = "a495ff22c5b14b44b5121370f02d74de",
    scratchThr = "a495ff23c5b14b44b5121370f02d74de",
    scratchFor = "a495ff24c5b14b44b5121370f02d74de",
    scratchFiv = "a495ff25c5b14b44b5121370f02d74de";

// ******
// Adjust based on however many scratch characteristics you need to read. 
// Up to the three listed above. (Comma separated within brackets below.)
var scratch = [scratchOne, scratchTwo];
// ******


/* | Dependencie
 ---|---------------------------------*/

var noble = require('noble');
var osc = require('osc-min');
var dgram = require('dgram');
var _ = require('lodash');

var currentOSC;

/* | OSC Werks
 ---|---------------------------------*/

var udp = dgram.createSocket("udp4");

var outport = 41234;
var inport = 5001;

console.log("OSC will be sent to: http://localhost:" + outport);

// Send Data Function

var sendDataToOSC = null;
var oscBuffer;
var buf = new Buffer(1);

sendDataToOSC = function (characteristic, data) {

    oscBuffer = osc.toBuffer({
        address: "/devices",
        args: [characteristic, data]
    });

    try {
        udp.send(oscBuffer, 0, oscBuffer.length, outport, "localhost");
    } catch (e) {
        console.log("Error Thrown:");
        console.log(e);
    }

    oscBuffer = null;

};

var getDataFromOSC = null;
var sock;

Number.prototype.map = function (in_min, in_max, out_min, out_max) {
    return (this - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

getDataFromOSC = function () {

    sock = dgram.createSocket("udp4", function (msg, rinfo) {
        var error;
        try {
            var actual_value = osc.fromBuffer(msg).args[0].value;
            actual_value = actual_value.map(-0.3,1,0,10);
            currentOSC = actual_value;
            buf.writeUInt8(currentOSC, 0);
        } catch (_error) {
            error = _error;
            return console.log("invalid OSC packet");
        }
    });

    sock.bind(inport);

};

getDataFromOSC();

/* | Bean communication
 ---|---------------------------------*/

var beanUUID = "a495ff10c5b14b44b5121370f02d74de";

var connectedBean = null;

// This function takes values from Bean characteristics. It waits for new data to
// come in and then sends that on to the OSC port.  

var writeTimer;

var subscribeToChars = function (characteristics) {

    //characteristics.forEach(function (characteristic, index) {

    var charWrite = characteristics[0];
    var charRead  = characteristics[1];

    charRead.on("read", function (data, sad) {

        // Can send four ints per scratch if you use: `data.toByteArray()`
        // instead of the byte shifting in `value` below.
        //var value = ;

        //console.log( data[0] );

    });

    charRead.notify(true, function (err) {
        if (err) throw err;
    });

    writeTimer = setInterval(function () {
        charWrite.write(buf, false, function (err) {
            //console.log(buf);
        });
    }, 100);


    //});

};

var setupChars = function (peripheral) {

    peripheral.discoverSomeServicesAndCharacteristics([], scratch, function (err, services, characteristics) {
        if (err) throw err;
        subscribeToChars(characteristics);
    });

};

var setupPeripheral = function (peripheral) {

    console.log('Connecting to ' + peripheral.advertisement.localName + '...');

    peripheral.connect(function (err) {
        if (err) throw err;

        console.log('Connected!');
        sendDataToOSC(3, 0);
        connectedBean = peripheral; // Sets the global to the Bean. Yuck.

        setupChars(connectedBean);

        connectedBean.on('disconnect', function () {
            console.log("The bean disconnected, trying to find it...");
            //sendDataToOSC(4, 0);
            //noble.startScanning();
        });

    });

};

noble.on('discover', function (peripheral) {

    if (_.contains(peripheral.advertisement.serviceUuids, beanUUID)) {
        console.log("Found a Bean");
        noble.stopScanning();
        console.log("Stopped " + "scanning.");

        // Once found, connect:
        setupPeripheral(peripheral);

    } else {
        console.log("Found a random BLE device, that is not a Bean, ignored.");
    }

});


noble.on('stateChange', function (state) {
    if (state == "poweredOn") {
        console.log("Started Scanning");
        noble.startScanning();
    }
});

process.stdin.resume(); //so the program will not close instantly


/* | Exit Handler
 | Disconnects from the bean, in order to reset BLE comms. */

var triedToExit = false;

function exitHandler(options, err) {
    if (connectedBean && !triedToExit) {
        triedToExit = true;
        console.log('Disconnecting from Bean...');
        connectedBean.disconnect(function (err) {
            console.log('Disconnected.');
            process.exit();
        });
    } else {
        process.exit();
    }
}

process.on('SIGINT', exitHandler.bind(null, {exit: true}));
