<?php
/**
 * @file
 * Custom login blocks prepended to Organizational nodes
 */



/**
 * Implements hook_views_query_alter
 * @param type $view
 * @param type $query
 */
function xcalsauthen_views_query_alter(&$view, &$query) {
  if ($view->name == 'organizations_vbo' && $view->current_display == 'references_3') {
    $qry = db_query("SELECT distinct n.nid, n.title, auth.* 
    	FROM {calsauthentargets} auth inner join {node} n on auth.nid = n.nid 
  	  WHERE auth.setting_name = :value
  	  order by n.title", array(":value" => "connector")
  	);
    $nid = array();
    foreach($qry as $row)  $nid[] = $row->nid;
    $query->add_where(0,'node.nid', $nid, 'IN');
  }
}

/**
 * Implements hook_form_node_type_form_alter
 */

function xcalsauthen_form_organization_node_form_alter(&$form, &$form_state, $form_id) {
  $qry = db_query("SELECT distinct n.nid, n.title, auth.* 
  	FROM {calsauthentargets} auth inner join {node} n on auth.nid = n.nid 
  	WHERE auth.setting_name = :value
  	order by n.title", array(":value" => "connector")
  );
  
  $options = array("" => t("-NONE-") );
  
  foreach($qry as $row) {
  	$options[$row->nid] = check_plain($row->title) . " [" . $row->tarid . "]";
  }
  $form['field_authentication_targetxx'][LANGUAGE_NONE]['#options'] = $options;
}


/**
 * Implements hook_block_view()
 */
function cals_forms_block_view($delta = '') {
	switch($delta) {
		//include this block on libraries where status = pending participant
  	case "cals_forms_temp_registration":
  	
      $block = array();
      global $user;
      if(arg(0) == 'node') {
        $node = node_load(arg(1));
        if($node->type == 'organization' && $node->field_partner_library_['und'][0]['value'] == 2) {
          $block['subject'] = '';
          if($user->uid == 0) {
	          $block['content'] =  '<p>' . 
	          	//t('@library is a future participant in the NNELS project pending final approval. You may apply for a registration. We will review and inform you if you ....:', array('@library' => $node->title) ) 
	          	//t('Logging in with your @library credentials is not yet enabled. Please fill in the form below for temporary access to NNELS in the meantime.', array('@library' => $node->title))
	            t('Logging in with your @library credentials is not yet enabled. Please fill in the form below to request access to NNELS.', array('@library' => $node->title))
	            . '</p>' . drupal_render(drupal_get_form('user_register_form'));
	      	  return $block;
          }
        }
      }
      break;
  
  	case "cals_forms_custom_registration":
  	
      $block = array();
      global $user;

      $block['subject'] = '';
      $block['content'] =  l(t('Sign Up'), "organizations/search", 
        array('attributes' => array('class' => array('sign-up-search')))); 
      $block['content'] .=  " ";
      $block['content'] .=  l(t("Login"), "user/login",
      	array(
      		"attributes" => array("class" => array("login-user") ),
      		"query" => array(drupal_get_destination()),
      	)
      );
    	return $block;
    	break;
  
	  case "cals_forms_library_login":
      $block = array();
      //check current node
      if (arg(0) == "node" and is_numeric(arg(1))) {
        $nid = arg(1);
        $is_partner = (function_exists('calsauthen_get_authentication_parent')) ? calsauthen_get_authentication_parent($nid) : 0;
  
        if ($is_partner > 0) {
          $node = node_load($nid);
					$form = drupal_get_form("user_login");
					$arr = array(
					  'field_username_label' => array('name', '#title'),
					  'field_password_label' => array('pass', '#title'),
					  'field_password_description' => array('pass','#description'),
					  'field_username_description' => array('name', '#description'),
					  'field_login_block_message' => array('message', '#markup'),
					);
					foreach($arr as $field => $fieldnames) {
					  $items = field_get_items('node', $node, $field);
					  $label = field_view_value('node', $node, $field, $items[0]);
					  if(!empty($label['#markup'])) $form[$fieldnames[0]][$fieldnames[1]] = render($label);
					}
					if($node->nid == 63265) {
					  $form['name']['#value'] = uniqid($prefix = "dev_");
					  //dpm($_SESSION);
					}  
					//dpm($form);
					
          $block['subject'] = ''; //blank for now. use t("") to add subject. Otherwise implement thru block.
          $block['content'] = '<div class="library-search-login">' . drupal_render($form) . '</div>';
					return $block;
        }
      }
      break;
	  case "cals_forms_staff_login":
			$block = array();
			$block['subject'] = ''; //blank for now. use t("") to add subject. Otherwise implement thru block.
			$form = drupal_get_form("user_login");
			$block['content'] = '<div class="library-search-login">' . drupal_render($form) . '</div>';
			return $block;
			//return "";
      break;
  }

}
