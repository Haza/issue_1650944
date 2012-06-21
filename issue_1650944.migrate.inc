<?php

class issue_1650944_Products extends Migration {
  public function __construct() {
    parent::__construct();
    $this->description = t('Import product from CSV file.');

    // Create a map object for tracking the relationships between source rows
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'sku' => array(
          'type' => 'varchar',
          'length' => 32,
          'not null' => TRUE,
        ),
      ),
      MigrateDestinationEntityAPI::getKeySchema('commerce_product')
    );

    // Create a MigrateSource object.
    $this->source = new MigrateSourceCSV(drupal_get_path('module', 'issue_1650944') . '/csv/products.csv', $this->csvcolumns(), array('header_rows' => 1));

    $this->destination = new MigrateDestinationEntityAPI('commerce_product', 'commerce_product');

    $this->addFieldMapping('title', 'title');
    $this->addFieldMapping('sku', 'sku');
    $this->addFieldMapping('commerce_price', 'price');
  }

  function csvcolumns() {
    $columns[0] = array('title', 'Title');
    $columns[1] = array('sku', 'SKU');
    $columns[2] = array('price', 'Price');
    return $columns;
  }

}

class issue_1650944_ProductDisplay extends Migration {

  public function __construct(array $arguments) {
    $this->arguments = $arguments;
    parent::__construct();
    $this->description = t('Import product nodes.');
    $this->dependencies = array('issue_1650944_Products');

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'title' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
        ),
      ),
      MigrateDestinationNode::getKeySchema()
    );

    // Create a MigrateSource object.
    $this->source = new MigrateSourceCSV(drupal_get_path('module', 'issue_1650944') . '/csv/products_display.csv', $this->csvcolumns(), array('header_rows' => 1));

    $this->destination = new MigrateDestinationNode('product_display');

    $this->addFieldMapping('uid', 'uid')->defaultValue(1);

    $this->addFieldMapping('title', 'title');
    $this->addFieldMapping('body', 'description');

    // See prepareRow
    $this->addFieldMapping('field_product_variations', 'skus');
  }

  public function prepareRow($row) {

    // The workaround.
    // Do not rely on a sourcemigration, but populate a pseudo-row with our
    // values.
    $products = array();
    foreach (explode(', ', $row->sku) as $sku) {
      $product = commerce_product_load_by_sku($sku);
      print_r($product);
      $products[] = $product->product_id;
    }
    $row->skus = $products;
  }

  function csvcolumns() {
    $columns[0] = array('title', 'Title');
    $columns[1] = array('description', 'Description');
    $columns[2] = array('sku', 'SKU');
    return $columns;
  }
}
