# Drupal Doctor

## Presentation
This project was created during one of my school project. We need to use our custom classes to develop some of our modules. The first problem was to connect our classes to the database using Drupal Database API. So I decided to make my own ORM dedicated to Drupal, and now I decide to publish my work.

## How does this work ?

### Installation

1. Simply install the module like every other Drupal module.
2. Create an empty folder ```/sites/all/libraries/doctor/```.
3. It's done

### Namespace and Filename

```php
namespace \App;

class TestingClass extends \Doctor{

}
```
The namespace of your class will determine the folder structure of your files.
This file needs to be in ```/sites/all/libraries/app/TestingClass.php```. Doctor will load this file by itself if you follow this restriction.

### Basic structure

```php
namespace \App;

class TestingClass extends \Doctor{

  protected static $table = 'aaa_testing';

  protected $fillable = array(
    'tid' => 'int',
    'name' => 'varchar',
    'description' => 'varchar',
    );


  protected $required = array(
    'tid', 'name'
    );

  protected $defaults = array(
    'description' => 'No description',
    );

  protected $unique = array(
    'name',
  );

  protected static $id = 'tid';
}
```
- ```$table``` is the database table name of your object.
- ```$fillable``` represents all fillable fields of your object. A field can be one of the 4 following types : ```ìnt```, ```double```, ```varchar``` and ```date```.
- ```$required``` are the fields that are required in your database.
- ```$defaults``` are the default values that will be put in the property if it's not set, null or empty.
- ```$unique``` are unique properties.
- ```$id``` is the name of the property that represents the ID field.

### Function

Let's say we have this object :
```php
namespace \App;

class TestingClass extends \Doctor{

  protected static $table = 'aaa_testing';

  protected $fillable = array(
    'tid' => 'int',
    'name' => 'varchar',
    'description' => 'varchar',
    );


  protected $required = array(
    'tid', 'name'
    );

  protected $defaults = array(
    'description' => 'No description',
    );

  protected $unique = array(
    'name',
  );

  protected static $id = 'tid';
  
  public function one()
  {
    return 1;
  }
}
```
We can call the ```one``` function by two ways : ```$test->one()``` or ```$test->one``` because this function doesn't take any parameter (if your function take one or more parameter(s), then you can only call it the normal way ```$test->two(2)```).
Doctor comes with a lot of pre-made functions :
- ```public static createSchema()``` will return the Drupal schema based on your class definition.
- ```public static findAll()``` will return a DoctorCollection object with all the rows from database.
- ```public static findAllByProperty(String $property, String prefix = null)``` will return a DoctorCollection object of all rows with the ID as key and the property as value. The prefix is for accessing the property of a property.
- ```public function moneyFormat($property, $symbol = TRUE, $moneySymbol = '€')``` will return a String of the property specified formatted in money. ```$symbol``` is to show or not the money symbol.
- ```public function yesOrNo(String $property)``` will return ```Yes``` if the property is equals to 1, ```No``` if 0, and ```null```otherwise.

### Database function
##### Create
```
$test = new \App\Test;
```

##### Find by ID
```
$test = \App\Test::find($id);
```

##### Delete
```
$test->delete();
```

##### Save changes
```
$test->save();
```

### Relationships
##### Belongs To
```
public function test2()
{
    return $this->belongsTo('\App\TestingClass2', 'fieldOnTestingClass', 'fieldOnTestingClass2');
}
```
##### Has Many
```
public function test3()
{
    return $this->hasMany('\App\TestingClass3', 'fieldOnTestingClass', 'fieldOnTestingClass3')->get();
}
```
##### Join
```
public function joinClass()
{
    return $this->join('\App\JoinClass', '\App\OtherClass', 'field')->get();
}
```