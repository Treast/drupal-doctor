<?php

class DoctorCollection
{
  protected $items = array();

  public function __construct($items = array())
  {
    $this->items = $items;
  }

  public function where($key, $value, $operator = '=')
  {
    $collection = new DoctorCollection();

    foreach($this->items as $item)
    {
      switch ($operator) {
        case '=':
          if($item->$key == $value)
          {
            $collection->insert($item);
          }
          break;
        case '<=':
          if($item->$key <= $value)
          {
            $collection->insert($item);
          }
          break;
        case '>=':
          if($item->$key >= $value)
          {
            $collection->insert($item);
          }
          break;
        case '<':
          if($item->$key < $value)
          {
            $collection->insert($item);
          }
          break;
        case '>':
          if($item->$key > $value)
          {
            $collection->insert($item);
          }
          break;
      }
    }
    return $collection;
  }

  public function orderBy($field, $order = 'ASC')
  {
    $collection = new DoctorCollection($this->items);
    switch ($order) {
      case 'DESC':
        usort($collection->items, function($a, $b) use ($field){
          return strcmp(strtolower($b->$field), strtolower($a->$field));
        });
        break;
      case 'NONE':
        usort($collection->items, function($a, $b) {
          return strcmp(strtolower($a), strtolower($b));
        });
        break;
      default:
        usort($collection->items, function($a, $b) use ($field){
          return strcmp(strtolower($a->$field), strtolower($b->$field));
        });
        break;
    }
    return $collection;
  }

  public function insert($item, $index = null)
  {
    if($index === null)
      $this->items[] = $item;
    else
      $this->items[$index] = $item;
    return $this;
  }

  public function get()
  {
    return $this->items;
  }

  public function first()
  {
    return reset($this->items);
  }

  public function last()
  {
    return end($this->items);
  }

  public function deleteAll($property = null)
  {
    foreach ($this->items as $item) {
      if(is_null($property))
        $item->delete();
      else
        $item->$property->delete();
    }
  }
}