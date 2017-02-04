<?php

class Doctor
{

  public static function find($id)
  {
    // Get the class that called this function
    $class = get_called_class();

    // Create an empty object that gonna be filled
    $object = new $class();

    // Query to the database
    $query = db_select($class::$table, 'x')
      ->fields('x')
      ->condition($class::$id, $id, '=');

    // Execution of the query
    $execute = $query->execute();

    // If there's only one record
    if($execute->rowCount() == 1) {

      // Fetch the record as an object
      $execute = $execute->fetchObject();
      foreach ($object->fillable as $key => $value) {
        if(isset($execute->$key))
          // Fill the object with each data from the record
          $object->$key = $execute->$key;
      }
      // Return of the object filled
      return $object;
    }
    // If none or more than 1 rows on database
    return null;
  }

  public function __get($field) {
    // If the class has this property
    if(property_exists($this, $field))
      return $this->$field;
    else {
      // If no property, check if a method has this name
      if(method_exists($this, $field))
        return $this->$field();
      else
        // If no property or method
        return null;
    }
  }

  public function isValid()
  {
    $result = TRUE;
    // Check if the class has required fields
    if(property_exists($this, 'required'))
    {
      foreach ($this->required as $property)
      {
        // If the required property is null or not set
        if (!isset($this->$property) || is_null($this->$property))
        {
          $result = FALSE;
        }
      }
    }
    return $result;
  }

  public function setDefaults()
  {
    // Check if the class has default value
    if(property_exists($this, 'defaults'))
    {
      foreach ($this->defaults as $property => $default)
      {
        // If the property is null or not set or empty, then put the default value in the property
        if (!isset($this->$property) || empty($this->$property) || is_null($this->$property))
          $this->$property = $default;
      }
    }
  }

  public function yesOrNo($property)
  {
    // Yes if 1, No if 0 and null otherwise
    switch ($this->$property) {
      case 0:
        return 'No';
        break;

      case 1:
        return 'Yes';
        break;
      
      default:
        return null;
        break;
    }
  }

  public function moneyFormat($property, $symbol = TRUE, $moneySymbol = '€')
  {
    // If the symbol is shown
    if($symbol)
      return number_format($this->$property, 2, ',', ''). ' '. $moneySymbol;
    // Or not
    else
      return number_format($this->$property, 2, ',', '');
  }

  public function save()
  {
    // Check if the object is valid (required fields are filled)
    if($this->isValid())
      // Set defaults values
      $this->setDefaults();
    // Get the class name
    $class = get_class($this);
    // Get the name of the property ID
    $id = $class::$id;

    // If the current instance has a value set on ID field of the class
    if(isset($this->$id) && !empty($this->$id))
    {

      // Update the current entry
      $query = db_update($class::$table);
      $fields = array();
      // Foreach property of the class
      foreach($this->fillable as $key => $value)
      {
        if(property_exists($this, $key))
          $fields[$key] = $this->$key;
      }
      $query->fields($fields);
      // Set the update query condition
      $query->condition($id, $this->$id);
      // Execute the query
      $query->execute();
    } else {
      // Insert a new entry on the database
      $query = db_insert($class::$table);
      $fields = array();
      // Foreach property of the class
      foreach($this->fillable as $key => $value)
      {
        if(property_exists($this, $key))
          $fields[$key] = $this->$key;
      }
      $query->fields($fields);
      // Execute the query
      $newId = $query->execute();

      // Set the instance ID with the new ID entry
      if($id !== FALSE)
        $this->$id = $newId;
    }
  }

  public function delete(){
    // Get the class name
    $class = get_class($this);
    // If the class hasn't an ID field
    if($class::$id === FALSE)
    {
      $query = db_delete($class::$table);
      // Foreach property of the class
      foreach($this->fillable as $key => $value)
      {
        // Condition on this value
        if(property_exists($this, $key))
          $query->condition($key, $this->$key);
      }
      // Execute the query
      $query->execute();
    } else {
      // Get the object ID
      $id = $class::$id;
      // Delete the row with this ID
      $query = db_delete($class::$table)
      ->condition($id, $this->$id)
      ->execute();
    }
  }

  public function belongsTo($class, $field, $otherField = null)
  {
    if($otherField === null)
      $otherField = $field;
    // Find the first record
    $collection = $class::findAll()->where($otherField, $this->$field, '=')->first();
    return $collection;
  }

  public function hasMany($class, $field, $otherField = null)
  {
    if($otherField === null)
      $otherField = $field;
    // Find all records
    $collection = $class::findAll()->where($otherField, $this->$field, '=');
    return $collection;
  }

  public function join($join, $class, $field)
  {
    $joins = $join::findAll()->where($field, $this->$field, '=')->get();
    $collection = new DoctorCollection();
    
    $id = $class::$id;

    foreach($joins as $j)
    {
      $collection->insert($class::find($j->$id));
    }
    return $collection;
  }

  public static function findAll() {
    $class = get_called_class();
    $query = db_select($class::$table, 'x')
    ->fields('x');

    $execute = $query->execute();

    $collection = new DoctorCollection();

    foreach($execute as $object) {
      if($class::$id === FALSE)
      {
        $obj = new $class();
        foreach ($obj->fillable as $key => $value) {
          if(isset($object->$key))
            $obj->$key = $object->$key;
        }
        $collection->insert($obj);
      } else {
        foreach ($object as $key => $value) {
          if($class::$id == $key){
            $collection->insert($class::find($value));
          }
        }
      }
    }
    return $collection;
  }

  public static function findAllByProperty($property, $prefix = null){
    $collection = new DoctorCollection();
    $class = get_called_class();
    $property = strtolower($property);
    $prototype = new $class;
    if(is_null($prefix))
    {
      $objects = $class::findAll()->get();
      $id = $class::$id;
      foreach($objects as $object)
      {
        if($object->$property !== null)
          $collection->insert($object->$property, $object->$id);
      }
    } else {
      $objects = $class::findAll()->get();
      $id = $class::$id;
      foreach($objects as $object)
      {
        if(is_object($object->$prefix))
        {
          if($object->$prefix->$property !== null)
            $collection->insert($object->$prefix->$property, $object->$id);
        }
      }
    }
    return $collection;
  }

  public static function createSchema()
  {
    $fields = array();
    $unique = array();

    $class = get_called_class();
    $prototype = new $class;

    foreach($prototype->fillable as $field => $type){
      if($field == $class::$id){
        $fields[$field] = array('type' => 'serial', 'unsigned' => TRUE, 'not null' => TRUE);
        $unique[$field] = array($field);
      }else{
        if(isset($type)){
          switch($type){
            case 'int': $fields[$field] = array('type' => 'int', 'default' => null); break;
            case 'double': $fields[$field] = array('type' => 'float', 'size' => 'big', 'default' => null); break;
            case 'color': $fields[$field] = array('type' => 'char', 'default' => null, 'length' => 6); break;
            case 'varchar': $fields[$field] = array('type' => 'varchar', 'length' => 255,'default' => ''); break;
            case 'date': $fields[$field] = array('mysql_type' => 'datetime'); break;
          }
        }
      }
    }
    if(isset($prototype->unique))
    {
      foreach($prototype->unique as $field)
      {
        $unique[$field] = array($field);
      }
    }

    $schema = array(
      'fields' => $fields,
      'unique keys' => $unique,
      );

    return $schema;
  }

  public static function debug($v){
    echo '<pre>'.print_r($v, TRUE).'</pre>';
  }
}