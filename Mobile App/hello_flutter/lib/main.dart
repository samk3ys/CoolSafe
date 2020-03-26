import 'dart:async';
import 'dart:convert' show utf8;
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:flutter_blue/flutter_blue.dart';

void main() => runApp(MyApp());

class MyApp extends StatelessWidget {
  // This widget is the root of your application. 
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Demo',
      theme: ThemeData(
        // This is the theme of your application.
        //
        // Try running your application with "flutter run". You'll see the
        // application has a blue toolbar. Then, without quitting the app, try
        // changing the primarySwatch below to Colors.green and then invoke
        // "hot reload" (press "r" in the console where you ran "flutter run",
        // or simply save your changes to "hot reload" in a Flutter IDE).
        // Notice that the counter didn't reset back to zero; the application
        // is not restarted.
        primarySwatch: Colors.indigo,
        //accentColor: Colors.green,
        scaffoldBackgroundColor: Color.fromRGBO(32, 32, 48, 1),
        textTheme: TextTheme(
          //title: 
          display1: TextStyle(color: Colors.white, fontSize: 36.0),
          body1: TextStyle(color: Colors.white, fontSize: 24.0),
        )
      ),
      home: MyHomePage(title: 'Flutter & BLE Demo'),
      debugShowCheckedModeBanner: false,
    );
  }
}

class MyHomePage extends StatefulWidget {
  MyHomePage({Key key, this.title}) : super(key: key);
  // This widget is the home page of your application. It is stateful, meaning
  // that it has a State object (defined below) that contains fields that affect
  // how it looks.

  // This class is the configuration for the state. It holds the values (in this
  // case the title) provided by the parent (in this case the App widget) and
  // used by the build method of the State. Fields in a Widget subclass are
  // always marked "final".

  final String title;

  @override
  _MyHomePageState createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  final String SERVICE_UUID = "6e400000-b5a3-f393-e0a9-e50e24dcca9e";
  final String TX_UUID = "6e400001-b5a3-f393-e0a9-e50e24dcca9e";
  final String RX_UUID = "6e400002-b5a3-f393-e0a9-e50e24dcca9e";
  final String TARGET_DEVICE_NAME = "Particle Xenon";

  FlutterBlue flutterBlue = FlutterBlue.instance;
  StreamSubscription<ScanResult> scanSubScription;

  BluetoothDevice targetDevice;
  BluetoothCharacteristic txCharacteristic;
  BluetoothCharacteristic rxCharacteristic;
  String data;  // Holds what is received thru RXCharacteristic

  String connectionText = 'Looking for connection...';
  Color bleColor = Colors.grey;
  bool _connected = false;

  @override
  void initState() {
    super.initState();
    startScan();
  }

  startScan() {
    setState(() {
      connectionText = "Start Scanning";
    });

    scanSubScription = flutterBlue.scan().listen((scanResult) {
      if (scanResult.device.name == TARGET_DEVICE_NAME) {
        print('DEVICE found');
        stopScan();
        setState(() {
          connectionText = "Found Target Device";
        });

        targetDevice = scanResult.device;
        connectToDevice();
      }
    }, onDone: () => stopScan());
  }

  stopScan() {
    scanSubScription?.cancel();
    scanSubScription = null;
  }

  connectToDevice() async {
    if (targetDevice == null) return;

    setState(() {
      connectionText = "Device Connecting";
    });

    await targetDevice.connect();
    print('DEVICE CONNECTED');
    setState(() {
      bleColor = Colors.blue;
      connectionText = "Device Connected";
      _connected = true;
    });

    discoverServices();
  }

  disconnectFromDevice() {
    if (targetDevice == null) return;

    targetDevice.disconnect();

    setState(() {
      bleColor = Colors.grey;
      connectionText = "Device Disconnected";
      _connected = false;
    });
  }

  discoverServices() async {
    if (targetDevice == null) return;

    List<BluetoothService> services = await targetDevice.discoverServices();
    services.forEach((service) {
      // do something with service
      if (service.uuid.toString() == SERVICE_UUID) {
        service.characteristics.forEach((characteristic) {
          if (characteristic.uuid.toString() == TX_UUID) {
            txCharacteristic = characteristic;
            writeData("Hello from Flutter Blue!");
            /*setState(() {
              connectionText = "Successfully transmitted to ${targetDevice.name}";
            });*/
          }
          if (characteristic.uuid.toString() == RX_UUID) {
            rxCharacteristic = characteristic;
            readData(data);
            print(data);
            /*setState(() {
              connectionText = "Successfully received from ${targetDevice.name}";
            });*/
          }
        });
      }
    });
  }

  writeData(String data) {
    if (txCharacteristic == null) return;

    List<int> bytes = utf8.encode(data);
    txCharacteristic.write(bytes);
  }

  readData(String data) async {
    if (rxCharacteristic == null) return;
    
    List<int> bytes = await rxCharacteristic.read();
    data = utf8.decode(bytes);
  }

  @override
  Widget build(BuildContext context) {
    // This method is rerun every time setState is called, for instance as done
    // by the _incrementCounter method above.
    //
    // The Flutter framework has been optimized to make rerunning build methods
    // fast, so that you can just rebuild anything that needs updating rather
    // than having to individually change instances of widgets.
    return Scaffold(
      appBar: AppBar(
        // Here we take the value from the MyHomePage object that was created by
        // the App.build method, and use it to set our appbar title.
        title: Text(widget.title),
      ),
      body: Center(
        // Center is a layout widget. It takes a single child and positions it
        // in the middle of the parent.
        child: Column(
          children: <Widget>[            
            Text('\n$connectionText\n',
              style: Theme.of(context).textTheme.display1,
              textAlign: TextAlign.center,
            ),
            Icon(
              //Icons.bluetooth,
              _connected ? 
                Icons.bluetooth : Icons.bluetooth_disabled,
              color: bleColor,
              size: 100.0,
            ),
            Text('\n'),
            FlatButton(
              color: Colors.cyan,
              splashColor: Colors.cyanAccent,
              child: Text('\nLook for a connection\n',
                style: Theme.of(context).textTheme.display1,
              ), 
              onPressed: () {
                connectToDevice();  // Try to connect
                if (_connected) {   // Go to demo page if it connects
                  Navigator.push(
                    context,
                    MaterialPageRoute(builder: (context) => DemoPage()),
                  );
                }
                // Otherwise just stay put
              },
            ),
          ],
        ),
      ),
    );
  }
}

class DemoPage extends StatefulWidget {
  DemoPage({Key key, this.title}) : super(key: key);
  // This widget is the home page of your application. It is stateful, meaning
  // that it has a State object (defined below) that contains fields that affect
  // how it looks.

  // This class is the configuration for the state. It holds the values (in this
  // case the title) provided by the parent (in this case the App widget) and
  // used by the build method of the State. Fields in a Widget subclass are
  // always marked "final".

  final String title;

  @override
  _DemoPageState createState() => _DemoPageState();
}

class _DemoPageState extends State<DemoPage> {
  double dividerSpacing = 10.0;
  int _counter = 0;

  void _sendMessage(int command) {
    setState(() {
      _counter = command;
    });
    
    // Reads characteristics
    var characteristics = service.characteristics;
    for(BluetoothCharacteristic c in characteristics) {
        List<int> value = await c.read();
        print(value);
    }
    // Writes to a characteristic
    await c.write([0x12, 0x34])
  }

  void _addUser() {
    setState(() {
      _counter = 1;
    });
  }

  @override
  Widget build(BuildContext context) {
    // This method is rerun every time setState is called, for instance as done
    // by the _incrementCounter method above.
    //
    // The Flutter framework has been optimized to make rerunning build methods
    // fast, so that you can just rebuild anything that needs updating rather
    // than having to individually change instances of widgets.
    return Scaffold(
      appBar: AppBar(
        // Here we take the value from the MyHomePage object that was created by
        // the App.build method, and use it to set our appbar title.
        //title: Text(widget.title),
        title: Text('Flutter & BLE Demo'),
      ),
      body: Center(
        // Center is a layout widget. It takes a single child and positions it
        // in the middle of the parent.
        child: ListView(
          children: <Widget>[
            new GestureDetector(
              onTap: () {
                _sendMessage(1);
                _addUser();
              },
              child:
                _buildButtonRow(
                Colors.green[500], 
                Icons.person_add, 
                ' ADD USER (1)',
                context
              ),
            ),
            Divider(height: dividerSpacing, color: Color.fromRGBO(0, 0, 200, 1),),
            new GestureDetector(
              onTap: () {
                _sendMessage(2);
              },
              child:
                _buildButtonRow(
                  Colors.grey[500], 
                  Icons.create, 
                  ' EDIT USER (2)',
                  context
                ),
            ),
            Divider(height: dividerSpacing, color: Color.fromRGBO(0, 0, 200, 1),),
            new GestureDetector(
              onTap: () {
                _sendMessage(3);
              },
              child:
                _buildButtonRow(
                  Colors.yellow[500], 
                  Icons.block, 
                  ' DISABLE USER (3)',
                  context
                ),
            ),
            Divider(height: dividerSpacing, color: Color.fromRGBO(0, 0, 200, 1),),
            new GestureDetector(
              onTap: () {
                _sendMessage(4);
              },
              child:
                _buildButtonRow(
                  Colors.red[500], 
                  Icons.delete_forever, 
                  ' REMOVE USER (4)',
                  context
                ),
            ),
            Divider(height: dividerSpacing, color: Color.fromRGBO(0, 0, 200, 1),),
            
            new Text('\nSent command: $_counter',
              style: Theme.of(context).textTheme.display1,
              textAlign: TextAlign.center,
            ),
            Icon(
              Icons.bluetooth,
              color: Colors.blue,
              size: 64.0,
            ),

            Text('\n\n'),
            GestureDetector(
              onTap: () {
                // Return to previous (home) page
                Navigator.pop(context);
              },
              child:
                _buildButtonRow(
                  Colors.white, 
                  Icons.arrow_back, 
                  'Go back',
                  context
                ),
            ),

          ],
        ),
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: _addUser,
        //onPressed: demoPage,  // navigate to demoPage
        tooltip: 'Add',
        child: Icon(Icons.add),
      ), // This trailing comma makes auto-formatting nicer for build methods.
    );
  }

}

Row _buildButtonRow(Color color, IconData icon, String label, BuildContext context) {
  return Row(
    
    //crossAxisAlignment: CrossAxisAlignment.center,
    //mainAxisAlignment: MainAxisAlignment.center,
    children: <Widget>[
      Icon(
        icon,
        color: color,
        size: 64.0,
      ),
      Expanded(
        child: Column(
          //mainAxisAlignment: MainAxisAlignment.center,
          crossAxisAlignment: CrossAxisAlignment.start,
          children: <Widget>[
            Text(
              label,
              //style: Theme.of(context).textTheme.display1,
            ),
          ],
        ),
      ),
    ]
  );
}
