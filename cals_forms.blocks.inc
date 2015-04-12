<?php


/**
 * Implements hook_block_view()
 */
function cals_forms_block_view($delta = '') {
	switch($delta) {
	  case "_cals_forms_test_block":
			$block = array();
			$block['subject'] = 'TEMP: Test block'; 
			$block['content'] = '<div class="library-search-login"><p>This is a simple test block called from cals_forms</p></div>';
			return $block;
      break;      
  }

}