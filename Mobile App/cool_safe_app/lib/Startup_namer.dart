import 'package:flutter/material.dart';
import 'package:flutter/widgets.dart';
//import 'package:video_player/video_player.dart';

createRegAlertDialog(BuildContext context){
  return showDialog(context: context, builder: (context){
    return AlertDialog(
      title: Text("Registration Start"),
      content: Image.asset("assets/images/RegistrationGuide.gif"),
      actions: <Widget>[
        MaterialButton(
          child: Text("Got it!"),
          onPressed: () {
            Navigator.pop(context);
          },
        )
      ],
    );
  });
}

//Use signal format ID/OPERATION/AUTHORIZED/NAME
void AddUser(BuildContext context){
  //send signal ID/ADD/YES/NAME
  createRegAlertDialog(context);
//  if (good scan)
  createNameAlertDialog(context).then((onValue){
//  Do something with onValue (for example:
//    return showDialog(context: context, builder: (context){
//      return AlertDialog(
//        title: Text(onValue),
//        actions: <Widget>[
//          MaterialButton(
//            child: Text("Test!"),
//            onPressed: () {
//              Navigator.pop(context);
//            },
//          )
//        ],
//      );
//    });
  });
} //AddUser()

void EditUser(){
  null;
  //EditUser()
}

void AbleUser(){
  null;
  //AbleUser()
}

void DeleteUser(){
  null;
  //DeleteUser()
}

Future<String> createNameAlertDialog(BuildContext context){
  TextEditingController nameCon = TextEditingController();
  return showDialog(context: context, builder: (context){
    return AlertDialog(
      title: Text("Registration Over"),
      content: TextField(
        controller: nameCon,
      ),
      actions: <Widget>[
        MaterialButton(
          child: Text("Hello World 2"),
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
          ),*/
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
          ),*/
        ],
      ),
    );
  }
} //title bar for anything not the homepage

class HomePage extends StatelessWidget {

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
              AddUser(context);
            },
          ),
          MaterialButton(
            child: Text('View Permissions'),
            onPressed: () {
              Navigator.push(
                context,
                MaterialPageRoute(builder: (context) => UsersPermissions()),
              );
            },
          ),
        ],
      ),
    );
  }
} //Homepage: new fingerprint button and view permissions button

/*class PermissionsButton extends Permissions {
  Widget build(BuildContext context) {
    MaterialButton(
      child: Text('Name'),
      onPressed: () {
        Navigator.push(
          context,
          MaterialPageRoute(builder: (context) => Indiv())
        );
      },
    );
  }
}

class UserPermissions extends StatefulWidget {
  @override
  UserPermissionsState createState() => UserPermissionsState();
}
List<Widget> usersList(BuildContext context) {
  List<Widget> users = new List();
  users.add(new MaterialButton(
    child: Text('Name1'),
    onPressed: () {
      Navigator.push(
          context,
          MaterialPageRoute(builder: (context) => Indiv())
      );
    },
  ));
  users.add(new MaterialButton(
    child: Text('Name2'),
    onPressed: () {
      Navigator.push(
        context,
        MaterialPageRoute(builder: (context) => Indiv())
      );
    },
  ));
  return users;
}*/ //irrelevant now... was trying a round-about way to do something simple

class UsersPermissions extends StatelessWidget {
  List<String> users = ['Sam', "Shae", "Steven", "Sylvester", "Tylor"];
  //retrieve user list through the BLE

  @override
  Widget build(BuildContext context) {
    // Material is a conceptual piece of paper on which the UI appears.
    return Scaffold(
      // Column is a vertical, linear layout.
      body: Column(
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
                  MaterialPageRoute(builder: (context) => IndivUser(users[id], id))
              );
            },
          ), // if not working try to get block of commented code above to work
        ],
      ),
      floatingActionButton: FloatingActionButton(
        child: Icon(Icons.add),
        onPressed: () {
          AddUser(context);
        },
      ),
    );
  }
} //Permissions page: button for each registered fingerprint/user

class IndivUser extends StatelessWidget {
  IndivUser(this.nameIt, this.id);
  final int id;
  final String nameIt;
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
              EditUser();
            },
            //send edit signal through BLE
            //edit name in array
          ),
          MaterialButton(
            child: Text('Disable/Enable'),
            onPressed: () {
              AbleUser();
            },
            //send disable/enable signal through BLE
          ),
          MaterialButton(
            child: Text('Delete'),
            onPressed: () {
              DeleteUser();
            },
            //send delete signal through BLE
            //remove name in array
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