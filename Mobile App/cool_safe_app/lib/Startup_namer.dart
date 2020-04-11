import 'package:flutter/material.dart';
import 'package:flutter/widgets.dart';

createRegAlertDialog(BuildContext context, List<String> users){
  return showDialog(context: context, builder: (context){
    return AlertDialog(
      title: Text("Registration Start"),
      content: Image.asset("assets/images/RegistrationGuide.gif"),
      actions: <Widget>[
        MaterialButton(
          child: Text("Got it!"),
          onPressed: () {
            Navigator.pop(context);
            //if (good scan)----BLE CODE
            createNameAlertDialog(context).then((onValue){
              users.add(onValue);
              createSignalAlertDialog(context, "Add");
            });
          },
        )
      ],
    );
  });
}

createEditAlertDialog(BuildContext context, int id, List<String> users){
  return showDialog(context: context, builder: (context){
    return AlertDialog(
      title: Text("New Username"),
      actions: <Widget>[
        MaterialButton(
          child: Text("OK!"),
          onPressed: () {
            Navigator.pop(context);
            createNameAlertDialog(context).then((onValue){
              users[id] = onValue;
            });
          },
        )
      ],
    );
  });
}

createSignalAlertDialog(BuildContext context, String command){
  return showDialog(context: context, builder: (context){
    return AlertDialog(
      title: Text("$command signal sent"),
      actions: <Widget>[
        MaterialButton(
          child: Text("OK!"),
          onPressed: () {
            Navigator.pop(context);
            if (command == "Delete") {
              Navigator.pop(context);
            }
          },
        )
      ],
    );
  });
}

//Use signal format ID/OPERATION/AUTHORIZED/NAME
void AddUser(BuildContext context, List<String> users){
  //send signal ID/ADD/YES/NAME----BLE CODE
  createRegAlertDialog(context, users);
}  //AddUser()

void EditUser(BuildContext context, int id, List<String> users){
  //send signal ID/EDIT/AUTHORIZED/NAME----BLE CODE
//  createEditAlertDialog(context, id, users);
  createNameAlertDialog(context).then((onValue){
    users[id] = onValue;
    createSignalAlertDialog(context, "Edit");
  });
}  //EditUser()

void AbleUser(BuildContext context){
  //send signal ID/ABLE/~(AUTHORIZED)/NAME----BLE CODE
  createSignalAlertDialog(context, "Disable/Enable");
}  //AbleUser()

void DeleteUser(BuildContext context, int id, List<String> users){
  //send signal ID/DELETE/AUTHORIZED/NAME----BLE CODE
  users.removeAt(id);
  createSignalAlertDialog(context, "Delete");
}  //DeleteUser()

Future<String> createNameAlertDialog(BuildContext context){
  TextEditingController nameCon = TextEditingController();
  return showDialog(context: context, builder: (context){
    return AlertDialog(
      title: Text("Please give a username"),
      content: TextField(
        controller: nameCon,
      ),
      actions: <Widget>[
        MaterialButton(
          child: Text("Done!"),
          onPressed: () {
            Navigator.of(context).pop(nameCon.text.toString());
          },
        )
      ],
    );
  });
}

class HomeBar extends StatelessWidget {
  HomeBar({this.title});
  // Fields in a Widget subclass are always marked "final".
  final Widget title;

  @override
  Widget build(BuildContext context) {
    return Container(
      height: 56.0, // in logical pixels
      padding: const EdgeInsets.symmetric(horizontal: 8.0),
      decoration: BoxDecoration(color: Colors.cyan[200]),
      // Row is a horizontal, linear layout.
      child: Row(
        // <Widget> is the type of items in the list.
        children: <Widget>[
          // Expanded expands its child to fill the available space.
          Expanded(
            child: title,
          ),
          /*IconButton(
            icon: Icon(Icons.menu),
            tooltip: 'Settings',
            onPressed: null,
          ),*/ //possible future addition
        ],
      ),
    );
  }
} //title bar for homepage only

class NotHomeBar extends StatelessWidget {
  NotHomeBar({this.title});
  // Fields in a Widget subclass are always marked "final".
  final Widget title;

  @override
  Widget build(BuildContext context) {
    return Container(
      height: 56.0, // in logical pixels
      padding: const EdgeInsets.symmetric(horizontal: 8.0),
      decoration: BoxDecoration(color: Colors.cyan[200]),
      // Row is a horizontal, linear layout.
      child: Row(
        // <Widget> is the type of items in the list.
        children: <Widget>[
          IconButton(
            icon: Icon(Icons.arrow_back),
            tooltip: 'Return',
            onPressed: (){
              Navigator.pop(context);
            },
          ),
          // Expanded expands its child to fill the available space.
          Expanded(
            child: title,
          ),
          /*IconButton(
            icon: Icon(Icons.menu),
            tooltip: 'Return',
            onPressed: null,
          ),*/ //possible future addition
        ],
      ),
    );
  }
} //title bar for anything not the homepage

class HomePage extends StatelessWidget {
  List<String> users = ['Sam', "Shae", "Steven", "Sylvester", "Tylor"];
  //retrieve user list through the BLE----BLE CODE

  @override
  Widget build(BuildContext context) {
    // Material is a conceptual piece of paper on which the UI appears.
    return Material(
      // Column is a vertical, linear layout.
      child: Column(
        children: <Widget>[
          HomeBar(
            title: Text(
              'Welcome',
              style: Theme.of(context).primaryTextTheme.title,
            ),
          ),
          MaterialButton(
            child: Text('Add New Fingerprint'),
            onPressed: () {
              AddUser(context, users);
            },
          ),
          MaterialButton(
            child: Text('View Permissions'),
            onPressed: () {
              Navigator.push(
                context,
                MaterialPageRoute(builder: (context) => UsersPermissions(users)),
              );
            },
          ),
        ],
      ),
    );
  }
} //Homepage: new fingerprint button and view permissions button

class UsersPermissions extends StatelessWidget {
  UsersPermissions(this.users);
  List<String> users;

  @override
  Widget build(BuildContext context) {
    // Material is a conceptual piece of paper on which the UI appears.
    return Scaffold(
      // Column is a vertical, linear layout.
      body: SingleChildScrollView(
        child: Column(
        children: <Widget>[
          NotHomeBar(
            title: Text(
              'Users Permissions',
              style: Theme.of(context).primaryTextTheme.title,
            ),
          ),
          for (int id = 0; id < users.length; id++) MaterialButton(
            child: Text(users[id]),
            onPressed: () {
              Navigator.push(
                  context,
                  MaterialPageRoute(builder: (context) => IndivUser(users[id], id, users))
              );
            },
          ),
        ],
      )),
      floatingActionButton: FloatingActionButton(
        child: Icon(Icons.add),
        onPressed: () {
          AddUser(context, users);
        },
      ),
    );
  }
} //Permissions page: button for each registered fingerprint/user

class IndivUser extends StatelessWidget {
  IndivUser(this.nameIt, this.id, this.users);
  int id;
  String nameIt;
  List<String> users;
  @override
  Widget build(BuildContext context) {
    // Material is a conceptual piece of paper on which the UI appears.
    return Material(
      // Column is a vertical, linear layout.
      child: Column(
        children: <Widget>[
          NotHomeBar(
            title: Text(
              nameIt,
              style: Theme.of(context).primaryTextTheme.title,
            ),
          ),
          MaterialButton(
            child: Text('Edit'),
            onPressed: () {
              EditUser(context, id, users);
            },
          ),
          MaterialButton(
            child: Text('Disable/Enable'),
            onPressed: () {
              AbleUser(context);
            },
          ),
          MaterialButton(
            child: Text('Delete'),
            onPressed: () {
              DeleteUser(context, id, users);
            },
          ),
        ],
      ),
    );
  }
} //Page for an individual user: button to edit, disable/enable permissions, and delete

void main() {
  runApp(MaterialApp(
    title: 'TaDaaaa', // used by the OS task switcher
    home: HomePage(),
  ));
}