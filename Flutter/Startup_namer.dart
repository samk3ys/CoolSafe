import 'package:flutter/material.dart';

class HomeBar extends StatelessWidget {
  HomeBar({this.title});

  // Fields in a Widget subclass are always marked "final".

  final Widget title;

  @override
  Widget build(BuildContext context) {
    return Container(
      height: 56.0, // in logical pixels
      padding: const EdgeInsets.symmetric(horizontal: 8.0),
      decoration: BoxDecoration(color: Colors.amber[700]),
      // Row is a horizontal, linear layout.
      child: Row(
        // <Widget> is the type of items in the list.
        children: <Widget>[
          IconButton(
            icon: Icon(Icons.arrow_back),
            tooltip: 'Return',
            onPressed: null,
          ),
          // Expanded expands its child to fill the available space.
          Expanded(
            child: title,
          ),
        ],
      ),
    );
  }
}

class TestBar extends StatelessWidget {
  TestBar({this.title});

  // Fields in a Widget subclass are always marked "final".

  final Widget title;

  @override
  Widget build(BuildContext context) {
    return Container(
      height: 56.0, // in logical pixels
      padding: const EdgeInsets.symmetric(horizontal: 8.0),
      decoration: BoxDecoration(color: Colors.amber[700]),
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
        ],
      ),
    );
  }
}

class MyScaffold extends StatelessWidget {
  //final List<String> names = ['Billy', 'Bob', 'Joe'];

  @override
  Widget build(BuildContext context) {
    // Material is a conceptual piece of paper on which the UI appears.
    return Material(
      // Column is a vertical, linear layout.
      child: Column(
        children: <Widget>[
          HomeBar(
            title: Text(
              'Hello',
              style: Theme.of(context).primaryTextTheme.title,
            ),
          ),
          MaterialButton(
            child: Text('Permissions'),
            onPressed: () {
              Navigator.push(
                context,
                MaterialPageRoute(builder: (context) => Permissions()),
              );
            },
          ),
        ],
      ),
    );
  }
}

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
}*/
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
}

class Permissions extends StatelessWidget {
  //final List<String> users = ['Billy', 'Bob', 'Joe'];

  @override
/*  Widget build(BuildContext context){
    return Material(
      child: new Column(
        usersList(context: context)
      ),
    }*/
//  PermissionsState createState() => PermissionsState();

  Widget build(BuildContext context) {
    // Material is a conceptual piece of paper on which the UI appears.
    return Material(
      // Column is a vertical, linear layout.
      child: Column(
        children: <Widget>[
          TestBar(
            title: Text(
              'Users Permissions',
              style: Theme.of(context).primaryTextTheme.title,
            ),
          ),
          if (1 == 1){MaterialButton(
            child: Text('Name'),
            onPressed: () {
              Navigator.push(
                context,
                MaterialPageRoute(builder: (context) => Indiv())
              );
           },
          ),}
        ],
      ),
    );
  }
}

class Indiv extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    // Material is a conceptual piece of paper on which the UI appears.
    return Material(
      // Column is a vertical, linear layout.
      child: Column(
        children: <Widget>[
          TestBar(
            title: Text(
              'User',
              style: Theme.of(context).primaryTextTheme.title,
            ),
          ),
          MaterialButton(
            child: Text('Edit'),
            onPressed: null,
          ),
          MaterialButton(
            child: Text('Disable/Enable'),
            onPressed: null,
          ),
          MaterialButton(
            child: Text('Delete'),
            onPressed: null,
          ),
        ],
      ),
    );
  }
}

void main() {
  runApp(MaterialApp(
    title: 'TaDaaaa', // used by the OS task switcher
    home: MyScaffold(),
  ));
}