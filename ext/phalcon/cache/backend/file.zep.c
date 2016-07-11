
#ifdef HAVE_CONFIG_H
#include "../../../ext_config.h"
#endif

#include <php.h>
#include "../../../php_ext.h"
#include "../../../ext.h"

#include <Zend/zend_operators.h>
#include <Zend/zend_exceptions.h>
#include <Zend/zend_interfaces.h>

#include "kernel/main.h"
#include "kernel/array.h"
#include "kernel/exception.h"
#include "kernel/memory.h"
#include "kernel/object.h"
#include "kernel/string.h"
#include "kernel/fcall.h"
#include "kernel/operators.h"
#include "kernel/concat.h"
#include "kernel/file.h"
#include "kernel/time.h"
#include "kernel/iterator.h"
#include "ext/spl/spl_directory.h"


/**
 * Phalcon\Cache\Backend\File
 *
 * Allows to cache output fragments using a file backend
 *
 *<code>
 *	//Cache the file for 2 days
 *	$frontendOptions = array(
 *		'lifetime' => 172800
 *	);
 *
 *  //Create a output cache
 *  $frontCache = \Phalcon\Cache\Frontend\Output($frontOptions);
 *
 *	//Set the cache directory
 *	$backendOptions = array(
 *		'cacheDir' => '../app/cache/'
 *	);
 *
 *  //Create the File backend
 *  $cache = new \Phalcon\Cache\Backend\File($frontCache, $backendOptions);
 *
 *	$content = $cache->start('my-cache');
 *	if ($content === null) {
 *  	echo '<h1>', time(), '</h1>';
 *  	$cache->save();
 *	} else {
 *		echo $content;
 *	}
 *</code>
 */
ZEPHIR_INIT_CLASS(Phalcon_Cache_Backend_File) {

	ZEPHIR_REGISTER_CLASS_EX(Phalcon\\Cache\\Backend, File, phalcon, cache_backend_file, phalcon_cache_backend_ce, phalcon_cache_backend_file_method_entry, 0);

	/**
	 * Default to false for backwards compatibility
	 *
	 * @var boolean
	 */
	zend_declare_property_bool(phalcon_cache_backend_file_ce, SL("_useSafeKey"), 0, ZEND_ACC_PRIVATE TSRMLS_CC);

	zend_class_implements(phalcon_cache_backend_file_ce TSRMLS_CC, 1, phalcon_cache_backendinterface_ce);
	return SUCCESS;

}

/**
 * Phalcon\Cache\Backend\File constructor
 *
 * @param	Phalcon\Cache\FrontendInterface frontend
 * @param	array options
 */
PHP_METHOD(Phalcon_Cache_Backend_File, __construct) {

	zend_bool _1$$6;
	int ZEPHIR_LAST_CALL_STATUS;
	zephir_fcall_cache_entry *_5 = NULL;
	zval *frontend, *options = NULL, *prefix = NULL, *safekey = NULL, *_0$$6, *_2$$6, *_3$$6, _4$$6;

	ZEPHIR_MM_GROW();
	zephir_fetch_params(1, 1, 1, &frontend, &options);

	if (!options) {
		options = ZEPHIR_GLOBAL(global_null);
	}


	if (!(zephir_array_isset_string(options, SS("cacheDir")))) {
		ZEPHIR_THROW_EXCEPTION_DEBUG_STR(phalcon_cache_exception_ce, "Cache directory must be specified with the option cacheDir", "phalcon/cache/backend/file.zep", 78);
		return;
	}
	ZEPHIR_OBS_VAR(safekey);
	if (zephir_array_isset_string_fetch(&safekey, options, SS("safekey"), 0 TSRMLS_CC)) {
		if (Z_TYPE_P(safekey) != IS_BOOL) {
			ZEPHIR_THROW_EXCEPTION_DEBUG_STR(phalcon_cache_exception_ce, "safekey option should be a boolean.", "phalcon/cache/backend/file.zep", 83);
			return;
		}
		zephir_update_property_this(this_ptr, SL("_useSafeKey"), safekey TSRMLS_CC);
	}
	ZEPHIR_OBS_VAR(prefix);
	if (zephir_array_isset_string_fetch(&prefix, options, SS("prefix"), 0 TSRMLS_CC)) {
		_0$$6 = zephir_fetch_nproperty_this(this_ptr, SL("_useSafeKey"), PH_NOISY_CC);
		_1$$6 = zephir_is_true(_0$$6);
		if (_1$$6) {
			ZEPHIR_INIT_VAR(_2$$6);
			ZEPHIR_INIT_VAR(_3$$6);
			ZEPHIR_SINIT_VAR(_4$$6);
			ZVAL_STRING(&_4$$6, "/[^a-zA-Z0-9_.-]+/", 0);
			zephir_preg_match(_3$$6, &_4$$6, prefix, _2$$6, 0, 0 , 0  TSRMLS_CC);
			_1$$6 = zephir_is_true(_3$$6);
		}
		if (_1$$6) {
			ZEPHIR_THROW_EXCEPTION_DEBUG_STR(phalcon_cache_exception_ce, "FileCache prefix should only use alphanumeric characters.", "phalcon/cache/backend/file.zep", 92);
			return;
		}
	}
	ZEPHIR_CALL_PARENT(NULL, phalcon_cache_backend_file_ce, this_ptr, "__construct", &_5, 107, frontend, options);
	zephir_check_call_status();
	ZEPHIR_MM_RESTORE();

}

/**
 * Returns a cached content
 *
 * @param int|string keyName
 * @param   int lifetime
 * @return  mixed
 */
PHP_METHOD(Phalcon_Cache_Backend_File, get) {

	zend_bool _0;
	zephir_fcall_cache_entry *_7 = NULL;
	int ZEPHIR_LAST_CALL_STATUS, createdTime = 0, ttl = 0;
	zval *keyName, *lifetime = NULL, *prefixedKey = NULL, *cacheDir = NULL, *cacheFile = NULL, *frontend = NULL, *lastLifetime = NULL, *cachedContent = NULL, *ret = NULL, *_1, *_2 = NULL, *_3, *_4$$5, *_10$$5 = NULL, *_11$$5 = NULL, *_5$$6 = NULL, *_6$$6, *_8$$7, *_9$$9 = NULL, *_12$$12, *_13$$13, *_14$$14 = NULL, *_15$$14, *_16$$16, *_17$$17, *_18$$17;

	ZEPHIR_MM_GROW();
	zephir_fetch_params(1, 1, 1, &keyName, &lifetime);

	if (!lifetime) {
		lifetime = ZEPHIR_GLOBAL(global_null);
	}


	_0 = Z_TYPE_P(lifetime) != IS_NULL;
	if (_0) {
		_0 = ZEPHIR_LT_LONG(lifetime, 1);
	}
	if (_0) {
		ZEPHIR_THROW_EXCEPTION_DEBUG_STR(phalcon_cache_exception_ce, "The lifetime must be at least 1 second", "phalcon/cache/backend/file.zep", 112);
		return;
	}
	_1 = zephir_fetch_nproperty_this(this_ptr, SL("_prefix"), PH_NOISY_CC);
	ZEPHIR_CALL_METHOD(&_2, this_ptr, "getkey", NULL, 0, keyName);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(prefixedKey);
	ZEPHIR_CONCAT_VV(prefixedKey, _1, _2);
	zephir_update_property_this(this_ptr, SL("_lastKey"), prefixedKey TSRMLS_CC);
	ZEPHIR_OBS_VAR(cacheDir);
	_3 = zephir_fetch_nproperty_this(this_ptr, SL("_options"), PH_NOISY_CC);
	if (!(zephir_array_isset_string_fetch(&cacheDir, _3, SS("cacheDir"), 0 TSRMLS_CC))) {
		ZEPHIR_THROW_EXCEPTION_DEBUG_STR(phalcon_cache_exception_ce, "Unexpected inconsistency in options", "phalcon/cache/backend/file.zep", 119);
		return;
	}
	ZEPHIR_INIT_VAR(cacheFile);
	ZEPHIR_CONCAT_VV(cacheFile, cacheDir, prefixedKey);
	if ((zephir_file_exists(cacheFile TSRMLS_CC) == SUCCESS) == 1) {
		ZEPHIR_OBS_VAR(frontend);
		zephir_read_property_this(&frontend, this_ptr, SL("_frontend"), PH_NOISY_CC);
		ZEPHIR_INIT_VAR(_4$$5);
		zephir_file_get_contents(_4$$5, cacheFile TSRMLS_CC);
		ZEPHIR_INIT_VAR(cachedContent);
		zephir_json_decode(cachedContent, &(cachedContent), _4$$5, zephir_get_intval(ZEPHIR_GLOBAL(global_true))  TSRMLS_CC);
		if (!(zephir_is_true(lifetime))) {
			ZEPHIR_INIT_VAR(_6$$6);
			ZVAL_STRING(_6$$6, "lifetime", ZEPHIR_TEMP_PARAM_COPY);
			ZEPHIR_CALL_METHOD(&_5$$6, this_ptr, "isvalidarray", &_7, 108, cachedContent, _6$$6);
			zephir_check_temp_parameter(_6$$6);
			zephir_check_call_status();
			if (zephir_is_true(_5$$6)) {
				ZEPHIR_OBS_VAR(_8$$7);
				zephir_array_fetch_string(&_8$$7, cachedContent, SL("lifetime"), PH_NOISY, "phalcon/cache/backend/file.zep", 135 TSRMLS_CC);
				ttl = zephir_get_intval(_8$$7);
			} else {
				ZEPHIR_OBS_VAR(lastLifetime);
				zephir_read_property_this(&lastLifetime, this_ptr, SL("_lastLifetime"), PH_NOISY_CC);
				if (!(zephir_is_true(lastLifetime))) {
					ZEPHIR_CALL_METHOD(&_9$$9, frontend, "getlifetime", NULL, 0);
					zephir_check_call_status();
					ttl = zephir_get_intval(_9$$9);
				} else {
					ttl = zephir_get_intval(lastLifetime);
				}
			}
		} else {
			ttl = zephir_get_intval(lifetime);
		}
		ZEPHIR_INIT_VAR(_11$$5);
		ZVAL_STRING(_11$$5, "created", ZEPHIR_TEMP_PARAM_COPY);
		ZEPHIR_CALL_METHOD(&_10$$5, this_ptr, "isvalidarray", &_7, 108, cachedContent, _11$$5);
		zephir_check_temp_parameter(_11$$5);
		zephir_check_call_status();
		if (!(zephir_is_true(_10$$5))) {
			ZEPHIR_INIT_VAR(_12$$12);
			zephir_filemtime(_12$$12, cacheFile TSRMLS_CC);
			createdTime = zephir_get_intval(_12$$12);
		} else {
			ZEPHIR_OBS_VAR(_13$$13);
			zephir_array_fetch_string(&_13$$13, cachedContent, SL("created"), PH_NOISY, "phalcon/cache/backend/file.zep", 150 TSRMLS_CC);
			createdTime = zephir_get_intval(_13$$13);
		}
		ZEPHIR_INIT_NVAR(_11$$5);
		zephir_time(_11$$5);
		if (!((zephir_get_numberval(_11$$5) - ttl) > createdTime)) {
			ZEPHIR_INIT_VAR(_15$$14);
			ZVAL_STRING(_15$$14, "content", ZEPHIR_TEMP_PARAM_COPY);
			ZEPHIR_CALL_METHOD(&_14$$14, this_ptr, "isvalidarray", &_7, 108, cachedContent, _15$$14);
			zephir_check_temp_parameter(_15$$14);
			zephir_check_call_status();
			if (!(zephir_is_true(_14$$14))) {
				ZEPHIR_INIT_NVAR(cachedContent);
				zephir_file_get_contents(cachedContent, cacheFile TSRMLS_CC);
			} else {
				zephir_array_fetch_string(&_16$$16, cachedContent, SL("content"), PH_NOISY | PH_READONLY, "phalcon/cache/backend/file.zep", 165 TSRMLS_CC);
				ZEPHIR_CPY_WRT(cachedContent, _16$$16);
			}
			if (ZEPHIR_IS_FALSE_IDENTICAL(cachedContent)) {
				ZEPHIR_INIT_VAR(_17$$17);
				object_init_ex(_17$$17, phalcon_cache_exception_ce);
				ZEPHIR_INIT_VAR(_18$$17);
				ZEPHIR_CONCAT_SVS(_18$$17, "Cache file ", cacheFile, " could not be opened");
				ZEPHIR_CALL_METHOD(NULL, _17$$17, "__construct", NULL, 9, _18$$17);
				zephir_check_call_status();
				zephir_throw_exception_debug(_17$$17, "phalcon/cache/backend/file.zep", 168 TSRMLS_CC);
				ZEPHIR_MM_RESTORE();
				return;
			}
			if (zephir_is_numeric(cachedContent)) {
				RETURN_CCTOR(cachedContent);
			} else {
				ZEPHIR_CALL_METHOD(&ret, frontend, "afterretrieve", NULL, 0, cachedContent);
				zephir_check_call_status();
				RETURN_CCTOR(ret);
			}
		} else {
			ZEPHIR_CALL_METHOD(NULL, this_ptr, "delete", NULL, 0, keyName);
			zephir_check_call_status();
			RETURN_MM_NULL();
		}
	}
	ZEPHIR_MM_RESTORE();

}

/**
 * Stores cached content into the file backend and stops the frontend
 *
 * @param int|string keyName
 * @param string content
 * @param int lifetime
 * @param boolean stopBuffer
 */
PHP_METHOD(Phalcon_Cache_Backend_File, save) {

	zval *_5$$14, *_7$$15;
	int ZEPHIR_LAST_CALL_STATUS, ttl = 0;
	zend_bool stopBuffer, _0;
	zval *keyName = NULL, *content = NULL, *lifetime = NULL, *stopBuffer_param = NULL, *lastKey = NULL, *frontend = NULL, *cacheDir = NULL, *isBuffering = NULL, *cacheFile = NULL, *cachedContent = NULL, *preparedContent = NULL, *status = NULL, *finalContent = NULL, *lastLifetime = NULL, *_3, *_1$$5, *_2$$5 = NULL, *_4$$11 = NULL, *_6$$14 = NULL, *_8$$15 = NULL, *_9$$16, *_10$$16;

	ZEPHIR_MM_GROW();
	zephir_fetch_params(1, 0, 4, &keyName, &content, &lifetime, &stopBuffer_param);

	if (!keyName) {
		keyName = ZEPHIR_GLOBAL(global_null);
	}
	if (!content) {
		content = ZEPHIR_GLOBAL(global_null);
	}
	if (!lifetime) {
		lifetime = ZEPHIR_GLOBAL(global_null);
	}
	if (!stopBuffer_param) {
		stopBuffer = 1;
	} else {
		stopBuffer = zephir_get_boolval(stopBuffer_param);
	}


	_0 = Z_TYPE_P(lifetime) != IS_NULL;
	if (_0) {
		_0 = ZEPHIR_LT_LONG(lifetime, 1);
	}
	if (_0) {
		ZEPHIR_THROW_EXCEPTION_DEBUG_STR(phalcon_cache_exception_ce, "The lifetime must be at least 1 second", "phalcon/cache/backend/file.zep", 205);
		return;
	}
	if (Z_TYPE_P(keyName) == IS_NULL) {
		ZEPHIR_OBS_VAR(lastKey);
		zephir_read_property_this(&lastKey, this_ptr, SL("_lastKey"), PH_NOISY_CC);
	} else {
		_1$$5 = zephir_fetch_nproperty_this(this_ptr, SL("_prefix"), PH_NOISY_CC);
		ZEPHIR_CALL_METHOD(&_2$$5, this_ptr, "getkey", NULL, 0, keyName);
		zephir_check_call_status();
		ZEPHIR_INIT_NVAR(lastKey);
		ZEPHIR_CONCAT_VV(lastKey, _1$$5, _2$$5);
	}
	if (!(zephir_is_true(lastKey))) {
		ZEPHIR_THROW_EXCEPTION_DEBUG_STR(phalcon_cache_exception_ce, "Cache must be started first", "phalcon/cache/backend/file.zep", 215);
		return;
	}
	ZEPHIR_OBS_VAR(frontend);
	zephir_read_property_this(&frontend, this_ptr, SL("_frontend"), PH_NOISY_CC);
	ZEPHIR_OBS_VAR(cacheDir);
	_3 = zephir_fetch_nproperty_this(this_ptr, SL("_options"), PH_NOISY_CC);
	if (!(zephir_array_isset_string_fetch(&cacheDir, _3, SS("cacheDir"), 0 TSRMLS_CC))) {
		ZEPHIR_THROW_EXCEPTION_DEBUG_STR(phalcon_cache_exception_ce, "Unexpected inconsistency in options", "phalcon/cache/backend/file.zep", 221);
		return;
	}
	ZEPHIR_INIT_VAR(cacheFile);
	ZEPHIR_CONCAT_VV(cacheFile, cacheDir, lastKey);
	if (Z_TYPE_P(content) == IS_NULL) {
		ZEPHIR_CALL_METHOD(&cachedContent, frontend, "getcontent", NULL, 0);
		zephir_check_call_status();
	} else {
		ZEPHIR_CPY_WRT(cachedContent, content);
	}
	ZEPHIR_CALL_METHOD(&preparedContent, frontend, "beforestore", NULL, 0, cachedContent);
	zephir_check_call_status();
	if (!(zephir_is_true(lifetime))) {
		ZEPHIR_OBS_VAR(lastLifetime);
		zephir_read_property_this(&lastLifetime, this_ptr, SL("_lastLifetime"), PH_NOISY_CC);
		if (!(zephir_is_true(lastLifetime))) {
			ZEPHIR_CALL_METHOD(&_4$$11, frontend, "getlifetime", NULL, 0);
			zephir_check_call_status();
			ttl = zephir_get_intval(_4$$11);
		} else {
			ttl = zephir_get_intval(lastLifetime);
		}
	} else {
		ttl = zephir_get_intval(lifetime);
	}
	if (!(zephir_is_numeric(cachedContent))) {
		ZEPHIR_INIT_VAR(_5$$14);
		zephir_create_array(_5$$14, 3, 0 TSRMLS_CC);
		ZEPHIR_INIT_VAR(_6$$14);
		zephir_time(_6$$14);
		zephir_array_update_string(&_5$$14, SL("created"), &_6$$14, PH_COPY | PH_SEPARATE);
		ZEPHIR_INIT_NVAR(_6$$14);
		ZVAL_LONG(_6$$14, ttl);
		zephir_array_update_string(&_5$$14, SL("lifetime"), &_6$$14, PH_COPY | PH_SEPARATE);
		zephir_array_update_string(&_5$$14, SL("content"), &preparedContent, PH_COPY | PH_SEPARATE);
		ZEPHIR_INIT_VAR(finalContent);
		zephir_json_encode(finalContent, &(finalContent), _5$$14, 0  TSRMLS_CC);
	} else {
		ZEPHIR_INIT_VAR(_7$$15);
		zephir_create_array(_7$$15, 3, 0 TSRMLS_CC);
		ZEPHIR_INIT_VAR(_8$$15);
		zephir_time(_8$$15);
		zephir_array_update_string(&_7$$15, SL("created"), &_8$$15, PH_COPY | PH_SEPARATE);
		ZEPHIR_INIT_NVAR(_8$$15);
		ZVAL_LONG(_8$$15, ttl);
		zephir_array_update_string(&_7$$15, SL("lifetime"), &_8$$15, PH_COPY | PH_SEPARATE);
		zephir_array_update_string(&_7$$15, SL("content"), &cachedContent, PH_COPY | PH_SEPARATE);
		ZEPHIR_INIT_NVAR(finalContent);
		zephir_json_encode(finalContent, &(finalContent), _7$$15, 0  TSRMLS_CC);
	}
	ZEPHIR_INIT_VAR(status);
	zephir_file_put_contents(status, cacheFile, finalContent TSRMLS_CC);
	if (ZEPHIR_IS_FALSE_IDENTICAL(status)) {
		ZEPHIR_INIT_VAR(_9$$16);
		object_init_ex(_9$$16, phalcon_cache_exception_ce);
		ZEPHIR_INIT_VAR(_10$$16);
		ZEPHIR_CONCAT_SVS(_10$$16, "Cache file ", cacheFile, " could not be written");
		ZEPHIR_CALL_METHOD(NULL, _9$$16, "__construct", NULL, 9, _10$$16);
		zephir_check_call_status();
		zephir_throw_exception_debug(_9$$16, "phalcon/cache/backend/file.zep", 254 TSRMLS_CC);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&isBuffering, frontend, "isbuffering", NULL, 0);
	zephir_check_call_status();
	if (stopBuffer == 1) {
		ZEPHIR_CALL_METHOD(NULL, frontend, "stop", NULL, 0);
		zephir_check_call_status();
	}
	if (ZEPHIR_IS_TRUE_IDENTICAL(isBuffering)) {
		zend_print_zval(cachedContent, 0);
	}
	if (0) {
		zephir_update_property_this(this_ptr, SL("_started"), ZEPHIR_GLOBAL(global_true) TSRMLS_CC);
	} else {
		zephir_update_property_this(this_ptr, SL("_started"), ZEPHIR_GLOBAL(global_false) TSRMLS_CC);
	}
	ZEPHIR_MM_RESTORE();

}

/**
 * Deletes a value from the cache by its key
 *
 * @param int|string keyName
 * @return boolean
 */
PHP_METHOD(Phalcon_Cache_Backend_File, delete) {

	int ZEPHIR_LAST_CALL_STATUS;
	zval *keyName, *cacheFile = NULL, *cacheDir = NULL, *_0, *_1, *_2 = NULL;

	ZEPHIR_MM_GROW();
	zephir_fetch_params(1, 1, 0, &keyName);



	ZEPHIR_OBS_VAR(cacheDir);
	_0 = zephir_fetch_nproperty_this(this_ptr, SL("_options"), PH_NOISY_CC);
	if (!(zephir_array_isset_string_fetch(&cacheDir, _0, SS("cacheDir"), 0 TSRMLS_CC))) {
		ZEPHIR_THROW_EXCEPTION_DEBUG_STR(phalcon_cache_exception_ce, "Unexpected inconsistency in options", "phalcon/cache/backend/file.zep", 281);
		return;
	}
	_1 = zephir_fetch_nproperty_this(this_ptr, SL("_prefix"), PH_NOISY_CC);
	ZEPHIR_CALL_METHOD(&_2, this_ptr, "getkey", NULL, 0, keyName);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(cacheFile);
	ZEPHIR_CONCAT_VVV(cacheFile, cacheDir, _1, _2);
	if ((zephir_file_exists(cacheFile TSRMLS_CC) == SUCCESS)) {
		ZEPHIR_RETURN_CALL_FUNCTION("unlink", NULL, 109, cacheFile);
		zephir_check_call_status();
		RETURN_MM();
	}
	RETURN_MM_BOOL(0);

}

/**
 * Query the existing cached keys
 *
 * @param string|int prefix
 * @return array
 */
PHP_METHOD(Phalcon_Cache_Backend_File, queryKeys) {

	zend_object_iterator *_1;
	zval *keys;
	int ZEPHIR_LAST_CALL_STATUS;
	zval *prefix = NULL, *item = NULL, *key = NULL, *cacheDir = NULL, *_0, *_2, *_3$$4 = NULL;

	ZEPHIR_MM_GROW();
	zephir_fetch_params(1, 0, 1, &prefix);

	if (!prefix) {
		prefix = ZEPHIR_GLOBAL(global_null);
	}


	ZEPHIR_INIT_VAR(keys);
	array_init(keys);
	ZEPHIR_OBS_VAR(cacheDir);
	_0 = zephir_fetch_nproperty_this(this_ptr, SL("_options"), PH_NOISY_CC);
	if (!(zephir_array_isset_string_fetch(&cacheDir, _0, SS("cacheDir"), 0 TSRMLS_CC))) {
		ZEPHIR_THROW_EXCEPTION_DEBUG_STR(phalcon_cache_exception_ce, "Unexpected inconsistency in options", "phalcon/cache/backend/file.zep", 304);
		return;
	}
	ZEPHIR_INIT_VAR(_2);
	object_init_ex(_2, spl_ce_DirectoryIterator);
	ZEPHIR_CALL_METHOD(NULL, _2, "__construct", NULL, 110, cacheDir);
	zephir_check_call_status();
	_1 = zephir_get_iterator(_2 TSRMLS_CC);
	_1->funcs->rewind(_1 TSRMLS_CC);
	for (;_1->funcs->valid(_1 TSRMLS_CC) == SUCCESS && !EG(exception); _1->funcs->move_forward(_1 TSRMLS_CC)) {
		{
			zval **ZEPHIR_TMP_ITERATOR_PTR;
			_1->funcs->get_current_data(_1, &ZEPHIR_TMP_ITERATOR_PTR TSRMLS_CC);
			ZEPHIR_CPY_WRT(item, (*ZEPHIR_TMP_ITERATOR_PTR));
		}
		ZEPHIR_CALL_METHOD(&_3$$4, item, "isdir", NULL, 0);
		zephir_check_call_status();
		if (likely(ZEPHIR_IS_FALSE_IDENTICAL(_3$$4))) {
			ZEPHIR_CALL_METHOD(&key, item, "getfilename", NULL, 0);
			zephir_check_call_status();
			if (Z_TYPE_P(prefix) != IS_NULL) {
				if (zephir_start_with(key, prefix, NULL)) {
					zephir_array_append(&keys, key, PH_SEPARATE, "phalcon/cache/backend/file.zep", 316);
				}
			} else {
				zephir_array_append(&keys, key, PH_SEPARATE, "phalcon/cache/backend/file.zep", 319);
			}
		}
	}
	_1->funcs->dtor(_1 TSRMLS_CC);
	RETURN_CTOR(keys);

}

/**
 * Checks if cache exists and it isn't expired
 *
 * @param string|int keyName
 * @param   int lifetime
 * @return boolean
 */
PHP_METHOD(Phalcon_Cache_Backend_File, exists) {

	zend_bool cacheFileExists = 0, _12$$6;
	zephir_fcall_cache_entry *_6 = NULL;
	int lifetime, ZEPHIR_LAST_CALL_STATUS, ttl = 0;
	zval *keyName = NULL, *lifetime_param = NULL, *lastKey = NULL, *prefix = NULL, *cacheFile = NULL, *cachedContent = NULL, *_0$$4 = NULL, *_1$$5, *_2$$5, *_3$$6, *_10$$6 = NULL, *_11$$6 = NULL, *_13$$6, *_4$$7 = NULL, *_5$$7, *_7$$8, *_8$$9, *_9$$9 = NULL, *_14$$12, *_15$$12;

	ZEPHIR_MM_GROW();
	zephir_fetch_params(1, 0, 2, &keyName, &lifetime_param);

	if (!keyName) {
		keyName = ZEPHIR_GLOBAL(global_null);
	}
	if (!lifetime_param) {
		lifetime = 0;
	} else {
		lifetime = zephir_get_intval(lifetime_param);
	}


	if (!(zephir_is_true(keyName))) {
		ZEPHIR_OBS_VAR(lastKey);
		zephir_read_property_this(&lastKey, this_ptr, SL("_lastKey"), PH_NOISY_CC);
	} else {
		ZEPHIR_OBS_VAR(prefix);
		zephir_read_property_this(&prefix, this_ptr, SL("_prefix"), PH_NOISY_CC);
		ZEPHIR_CALL_METHOD(&_0$$4, this_ptr, "getkey", NULL, 0, keyName);
		zephir_check_call_status();
		ZEPHIR_INIT_NVAR(lastKey);
		ZEPHIR_CONCAT_VV(lastKey, prefix, _0$$4);
	}
	if (zephir_is_true(lastKey)) {
		_1$$5 = zephir_fetch_nproperty_this(this_ptr, SL("_options"), PH_NOISY_CC);
		zephir_array_fetch_string(&_2$$5, _1$$5, SL("cacheDir"), PH_NOISY | PH_READONLY, "phalcon/cache/backend/file.zep", 349 TSRMLS_CC);
		ZEPHIR_INIT_VAR(cacheFile);
		ZEPHIR_CONCAT_VV(cacheFile, _2$$5, lastKey);
		cacheFileExists = (zephir_file_exists(cacheFile TSRMLS_CC) == SUCCESS);
		if (cacheFileExists) {
			ZEPHIR_INIT_VAR(_3$$6);
			zephir_file_get_contents(_3$$6, cacheFile TSRMLS_CC);
			ZEPHIR_INIT_VAR(cachedContent);
			zephir_json_decode(cachedContent, &(cachedContent), _3$$6, zephir_get_intval(ZEPHIR_GLOBAL(global_true))  TSRMLS_CC);
			if (!(lifetime)) {
				ZEPHIR_INIT_VAR(_5$$7);
				ZVAL_STRING(_5$$7, "lifetime", ZEPHIR_TEMP_PARAM_COPY);
				ZEPHIR_CALL_METHOD(&_4$$7, this_ptr, "isvalidarray", &_6, 108, cachedContent, _5$$7);
				zephir_check_temp_parameter(_5$$7);
				zephir_check_call_status();
				if (zephir_is_true(_4$$7)) {
					ZEPHIR_OBS_VAR(_7$$8);
					zephir_array_fetch_string(&_7$$8, cachedContent, SL("lifetime"), PH_NOISY, "phalcon/cache/backend/file.zep", 358 TSRMLS_CC);
					ttl = zephir_get_intval(_7$$8);
				} else {
					_8$$9 = zephir_fetch_nproperty_this(this_ptr, SL("_frontend"), PH_NOISY_CC);
					ZEPHIR_CALL_METHOD(&_9$$9, _8$$9, "getlifetime", NULL, 0);
					zephir_check_call_status();
					ttl = zephir_get_intval(_9$$9);
				}
			} else {
				ttl = lifetime;
			}
			ZEPHIR_INIT_VAR(_11$$6);
			ZVAL_STRING(_11$$6, "created", ZEPHIR_TEMP_PARAM_COPY);
			ZEPHIR_CALL_METHOD(&_10$$6, this_ptr, "isvalidarray", &_6, 108, cachedContent, _11$$6);
			zephir_check_temp_parameter(_11$$6);
			zephir_check_call_status();
			_12$$6 = !zephir_is_true(_10$$6);
			if (_12$$6) {
				ZEPHIR_INIT_NVAR(_11$$6);
				zephir_filemtime(_11$$6, cacheFile TSRMLS_CC);
				ZEPHIR_INIT_VAR(_13$$6);
				zephir_time(_13$$6);
				_12$$6 = ZEPHIR_LT_LONG(_13$$6, (zephir_get_numberval(_11$$6) + ttl));
			}
			if (_12$$6) {
				RETURN_MM_BOOL(1);
			} else {
				zephir_array_fetch_string(&_14$$12, cachedContent, SL("created"), PH_NOISY | PH_READONLY, "phalcon/cache/backend/file.zep", 369 TSRMLS_CC);
				ZEPHIR_INIT_VAR(_15$$12);
				zephir_time(_15$$12);
				if (ZEPHIR_LT_LONG(_15$$12, (zephir_get_numberval(_14$$12) + ttl))) {
					RETURN_MM_BOOL(1);
				}
			}
		}
	}
	if (cacheFileExists) {
		ZEPHIR_CALL_METHOD(NULL, this_ptr, "delete", NULL, 0, keyName);
		zephir_check_call_status();
	}
	RETURN_MM_BOOL(0);

}

/**
 * Check if given variable is array, containing the key $cacheKey
 *
 * @param array|null cachedContent
 * @param string|null cacheKey
 * @return bool
 */
PHP_METHOD(Phalcon_Cache_Backend_File, isValidArray) {

	zend_bool _0;
	zval *cachedContent = NULL, *cacheKey = NULL;

	zephir_fetch_params(0, 0, 2, &cachedContent, &cacheKey);

	if (!cachedContent) {
		cachedContent = ZEPHIR_GLOBAL(global_null);
	}
	if (!cacheKey) {
		cacheKey = ZEPHIR_GLOBAL(global_null);
	}


	_0 = Z_TYPE_P(cachedContent) == IS_ARRAY;
	if (_0) {
		_0 = zephir_array_key_exists(cachedContent, cacheKey TSRMLS_CC);
	}
	RETURN_BOOL(_0);

}

/**
 * Increment of a given key, by number $value
 *
 * @param  string|int keyName
 * @param  int value
 * @return mixed
 */
PHP_METHOD(Phalcon_Cache_Backend_File, increment) {

	zval *_18$$14;
	zephir_fcall_cache_entry *_7 = NULL;
	int value, ZEPHIR_LAST_CALL_STATUS, modifiedTime = 0, ttl = 0;
	zval *keyName = NULL, *value_param = NULL, *prefixedKey = NULL, *cacheFile = NULL, *frontend = NULL, *cachedContent = NULL, *result = NULL, *lastLifetime = NULL, *newValue = NULL, *_0, *_1 = NULL, *_2, *_3, *_4$$3, *_5$$3 = NULL, *_6$$3 = NULL, *_10$$3 = NULL, *_8$$4, *_9$$6 = NULL, *_11$$8, *_12$$9, *_13$$10 = NULL, *_14$$10, *_15$$12, *_16$$13, *_17$$13, *_19$$14 = NULL, *_20$$18, *_21$$18;

	ZEPHIR_MM_GROW();
	zephir_fetch_params(1, 0, 2, &keyName, &value_param);

	if (!keyName) {
		keyName = ZEPHIR_GLOBAL(global_null);
	}
	if (!value_param) {
		value = 1;
	} else {
		value = zephir_get_intval(value_param);
	}


	_0 = zephir_fetch_nproperty_this(this_ptr, SL("_prefix"), PH_NOISY_CC);
	ZEPHIR_CALL_METHOD(&_1, this_ptr, "getkey", NULL, 0, keyName);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(prefixedKey);
	ZEPHIR_CONCAT_VV(prefixedKey, _0, _1);
	zephir_update_property_this(this_ptr, SL("_lastKey"), prefixedKey TSRMLS_CC);
	_2 = zephir_fetch_nproperty_this(this_ptr, SL("_options"), PH_NOISY_CC);
	zephir_array_fetch_string(&_3, _2, SL("cacheDir"), PH_NOISY | PH_READONLY, "phalcon/cache/backend/file.zep", 411 TSRMLS_CC);
	ZEPHIR_INIT_VAR(cacheFile);
	ZEPHIR_CONCAT_VV(cacheFile, _3, prefixedKey);
	if ((zephir_file_exists(cacheFile TSRMLS_CC) == SUCCESS)) {
		ZEPHIR_OBS_VAR(frontend);
		zephir_read_property_this(&frontend, this_ptr, SL("_frontend"), PH_NOISY_CC);
		ZEPHIR_INIT_VAR(_4$$3);
		zephir_file_get_contents(_4$$3, cacheFile TSRMLS_CC);
		ZEPHIR_INIT_VAR(cachedContent);
		zephir_json_decode(cachedContent, &(cachedContent), _4$$3, zephir_get_intval(ZEPHIR_GLOBAL(global_true))  TSRMLS_CC);
		ZEPHIR_INIT_VAR(_6$$3);
		ZVAL_STRING(_6$$3, "lifetime", ZEPHIR_TEMP_PARAM_COPY);
		ZEPHIR_CALL_METHOD(&_5$$3, this_ptr, "isvalidarray", &_7, 108, cachedContent, _6$$3);
		zephir_check_temp_parameter(_6$$3);
		zephir_check_call_status();
		if (zephir_is_true(_5$$3)) {
			ZEPHIR_OBS_VAR(_8$$4);
			zephir_array_fetch_string(&_8$$4, cachedContent, SL("lifetime"), PH_NOISY, "phalcon/cache/backend/file.zep", 427 TSRMLS_CC);
			ttl = zephir_get_intval(_8$$4);
		} else {
			ZEPHIR_OBS_VAR(lastLifetime);
			zephir_read_property_this(&lastLifetime, this_ptr, SL("_lastLifetime"), PH_NOISY_CC);
			if (!(zephir_is_true(lastLifetime))) {
				ZEPHIR_CALL_METHOD(&_9$$6, frontend, "getlifetime", NULL, 0);
				zephir_check_call_status();
				ttl = zephir_get_intval(_9$$6);
			} else {
				ttl = zephir_get_intval(lastLifetime);
			}
		}
		ZEPHIR_INIT_NVAR(_6$$3);
		ZVAL_STRING(_6$$3, "created", ZEPHIR_TEMP_PARAM_COPY);
		ZEPHIR_CALL_METHOD(&_10$$3, this_ptr, "isvalidarray", &_7, 108, cachedContent, _6$$3);
		zephir_check_temp_parameter(_6$$3);
		zephir_check_call_status();
		if (!(zephir_is_true(_10$$3))) {
			ZEPHIR_INIT_VAR(_11$$8);
			zephir_filemtime(_11$$8, cacheFile TSRMLS_CC);
			modifiedTime = zephir_get_intval(_11$$8);
		} else {
			ZEPHIR_OBS_VAR(_12$$9);
			zephir_array_fetch_string(&_12$$9, cachedContent, SL("created"), PH_NOISY, "phalcon/cache/backend/file.zep", 440 TSRMLS_CC);
			modifiedTime = zephir_get_intval(_12$$9);
		}
		ZEPHIR_INIT_NVAR(_6$$3);
		zephir_time(_6$$3);
		if (!((zephir_get_numberval(_6$$3) - ttl) > modifiedTime)) {
			ZEPHIR_INIT_VAR(_14$$10);
			ZVAL_STRING(_14$$10, "content", ZEPHIR_TEMP_PARAM_COPY);
			ZEPHIR_CALL_METHOD(&_13$$10, this_ptr, "isvalidarray", &_7, 108, cachedContent, _14$$10);
			zephir_check_temp_parameter(_14$$10);
			zephir_check_call_status();
			if (!(zephir_is_true(_13$$10))) {
				ZEPHIR_INIT_NVAR(cachedContent);
				zephir_file_get_contents(cachedContent, cacheFile TSRMLS_CC);
			} else {
				zephir_array_fetch_string(&_15$$12, cachedContent, SL("content"), PH_NOISY | PH_READONLY, "phalcon/cache/backend/file.zep", 454 TSRMLS_CC);
				ZEPHIR_CPY_WRT(cachedContent, _15$$12);
			}
			if (ZEPHIR_IS_FALSE_IDENTICAL(cachedContent)) {
				ZEPHIR_INIT_VAR(_16$$13);
				object_init_ex(_16$$13, phalcon_cache_exception_ce);
				ZEPHIR_INIT_VAR(_17$$13);
				ZEPHIR_CONCAT_SVS(_17$$13, "Cache file ", cacheFile, " could not be opened");
				ZEPHIR_CALL_METHOD(NULL, _16$$13, "__construct", NULL, 9, _17$$13);
				zephir_check_call_status();
				zephir_throw_exception_debug(_16$$13, "phalcon/cache/backend/file.zep", 458 TSRMLS_CC);
				ZEPHIR_MM_RESTORE();
				return;
			}
			if (zephir_is_numeric(cachedContent)) {
				ZEPHIR_INIT_VAR(newValue);
				ZVAL_LONG(newValue, (zephir_get_numberval(cachedContent) + value));
				ZEPHIR_INIT_VAR(_18$$14);
				zephir_create_array(_18$$14, 3, 0 TSRMLS_CC);
				ZEPHIR_INIT_VAR(_19$$14);
				zephir_time(_19$$14);
				zephir_array_update_string(&_18$$14, SL("created"), &_19$$14, PH_COPY | PH_SEPARATE);
				ZEPHIR_INIT_NVAR(_19$$14);
				ZVAL_LONG(_19$$14, ttl);
				zephir_array_update_string(&_18$$14, SL("lifetime"), &_19$$14, PH_COPY | PH_SEPARATE);
				zephir_array_update_string(&_18$$14, SL("content"), &newValue, PH_COPY | PH_SEPARATE);
				ZEPHIR_INIT_VAR(result);
				zephir_json_encode(result, &(result), _18$$14, 0  TSRMLS_CC);
				ZEPHIR_INIT_NVAR(_19$$14);
				zephir_file_put_contents(_19$$14, cacheFile, result TSRMLS_CC);
				if (ZEPHIR_IS_FALSE_IDENTICAL(_19$$14)) {
					ZEPHIR_THROW_EXCEPTION_DEBUG_STR(phalcon_cache_exception_ce, "Cache directory could not be written", "phalcon/cache/backend/file.zep", 465);
					return;
				}
				RETURN_CCTOR(newValue);
			} else {
				ZEPHIR_THROW_EXCEPTION_DEBUG_STR(phalcon_cache_exception_ce, "The cache value is not numeric, therefore could not be incremented", "phalcon/cache/backend/file.zep", 470);
				return;
			}
		} else {
			ZEPHIR_CALL_METHOD(NULL, this_ptr, "delete", NULL, 0, keyName);
			zephir_check_call_status();
			RETURN_MM_NULL();
		}
	} else {
		ZEPHIR_INIT_VAR(_20$$18);
		object_init_ex(_20$$18, phalcon_cache_exception_ce);
		ZEPHIR_INIT_VAR(_21$$18);
		ZEPHIR_CONCAT_SVS(_21$$18, "Cache file ", cacheFile, " could not be opened");
		ZEPHIR_CALL_METHOD(NULL, _20$$18, "__construct", NULL, 9, _21$$18);
		zephir_check_call_status();
		zephir_throw_exception_debug(_20$$18, "phalcon/cache/backend/file.zep", 480 TSRMLS_CC);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_MM_RESTORE();

}

/**
 * Decrement of a given key, by number $value
 *
 * @param  string|int keyName
 * @param  int value
 * @return mixed
 */
PHP_METHOD(Phalcon_Cache_Backend_File, decrement) {

	zval *_20$$16;
	zephir_fcall_cache_entry *_7 = NULL;
	int value, ZEPHIR_LAST_CALL_STATUS, ttl = 0, modifiedTime = 0, lifetime = 0;
	zval *keyName = NULL, *value_param = NULL, *prefixedKey = NULL, *cacheFile = NULL, *cachedContent = NULL, *result = NULL, *lastLifetime = NULL, *newValue = NULL, *_0, *_1 = NULL, *_2, *_3, *_4$$3, *_11$$3 = NULL, *_12$$3 = NULL, *_5$$4 = NULL, *_6$$4, *_8$$5, *_9$$7, *_10$$7 = NULL, *_13$$10, *_14$$11, *_15$$12 = NULL, *_16$$12, *_17$$14, *_18$$15, *_19$$15, *_21$$16 = NULL, *_22$$20, *_23$$20;

	ZEPHIR_MM_GROW();
	zephir_fetch_params(1, 0, 2, &keyName, &value_param);

	if (!keyName) {
		keyName = ZEPHIR_GLOBAL(global_null);
	}
	if (!value_param) {
		value = 1;
	} else {
		value = zephir_get_intval(value_param);
	}


	_0 = zephir_fetch_nproperty_this(this_ptr, SL("_prefix"), PH_NOISY_CC);
	ZEPHIR_CALL_METHOD(&_1, this_ptr, "getkey", NULL, 0, keyName);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(prefixedKey);
	ZEPHIR_CONCAT_VV(prefixedKey, _0, _1);
	zephir_update_property_this(this_ptr, SL("_lastKey"), prefixedKey TSRMLS_CC);
	_2 = zephir_fetch_nproperty_this(this_ptr, SL("_options"), PH_NOISY_CC);
	zephir_array_fetch_string(&_3, _2, SL("cacheDir"), PH_NOISY | PH_READONLY, "phalcon/cache/backend/file.zep", 498 TSRMLS_CC);
	ZEPHIR_INIT_VAR(cacheFile);
	ZEPHIR_CONCAT_VV(cacheFile, _3, prefixedKey);
	if ((zephir_file_exists(cacheFile TSRMLS_CC) == SUCCESS)) {
		ZEPHIR_INIT_VAR(_4$$3);
		zephir_file_get_contents(_4$$3, cacheFile TSRMLS_CC);
		ZEPHIR_INIT_VAR(cachedContent);
		zephir_json_decode(cachedContent, &(cachedContent), _4$$3, zephir_get_intval(ZEPHIR_GLOBAL(global_true))  TSRMLS_CC);
		if (!(lifetime)) {
			ZEPHIR_INIT_VAR(_6$$4);
			ZVAL_STRING(_6$$4, "lifetime", ZEPHIR_TEMP_PARAM_COPY);
			ZEPHIR_CALL_METHOD(&_5$$4, this_ptr, "isvalidarray", &_7, 108, cachedContent, _6$$4);
			zephir_check_temp_parameter(_6$$4);
			zephir_check_call_status();
			if (zephir_is_true(_5$$4)) {
				ZEPHIR_OBS_VAR(_8$$5);
				zephir_array_fetch_string(&_8$$5, cachedContent, SL("lifetime"), PH_NOISY, "phalcon/cache/backend/file.zep", 512 TSRMLS_CC);
				ttl = zephir_get_intval(_8$$5);
			} else {
				ZEPHIR_OBS_VAR(lastLifetime);
				zephir_read_property_this(&lastLifetime, this_ptr, SL("_lastLifetime"), PH_NOISY_CC);
				if (!(zephir_is_true(lastLifetime))) {
					_9$$7 = zephir_fetch_nproperty_this(this_ptr, SL("_frontend"), PH_NOISY_CC);
					ZEPHIR_CALL_METHOD(&_10$$7, _9$$7, "getlifetime", NULL, 0);
					zephir_check_call_status();
					ttl = zephir_get_intval(_10$$7);
				} else {
					ttl = zephir_get_intval(lastLifetime);
				}
			}
		} else {
			ttl = lifetime;
		}
		ZEPHIR_INIT_VAR(_12$$3);
		ZVAL_STRING(_12$$3, "created", ZEPHIR_TEMP_PARAM_COPY);
		ZEPHIR_CALL_METHOD(&_11$$3, this_ptr, "isvalidarray", &_7, 108, cachedContent, _12$$3);
		zephir_check_temp_parameter(_12$$3);
		zephir_check_call_status();
		if (!(zephir_is_true(_11$$3))) {
			ZEPHIR_INIT_VAR(_13$$10);
			zephir_filemtime(_13$$10, cacheFile TSRMLS_CC);
			modifiedTime = zephir_get_intval(_13$$10);
		} else {
			ZEPHIR_OBS_VAR(_14$$11);
			zephir_array_fetch_string(&_14$$11, cachedContent, SL("created"), PH_NOISY, "phalcon/cache/backend/file.zep", 527 TSRMLS_CC);
			modifiedTime = zephir_get_intval(_14$$11);
		}
		ZEPHIR_INIT_NVAR(_12$$3);
		zephir_time(_12$$3);
		if (!((zephir_get_numberval(_12$$3) - ttl) > modifiedTime)) {
			ZEPHIR_INIT_VAR(_16$$12);
			ZVAL_STRING(_16$$12, "content", ZEPHIR_TEMP_PARAM_COPY);
			ZEPHIR_CALL_METHOD(&_15$$12, this_ptr, "isvalidarray", &_7, 108, cachedContent, _16$$12);
			zephir_check_temp_parameter(_16$$12);
			zephir_check_call_status();
			if (!(zephir_is_true(_15$$12))) {
				ZEPHIR_INIT_NVAR(cachedContent);
				zephir_file_get_contents(cachedContent, cacheFile TSRMLS_CC);
			} else {
				zephir_array_fetch_string(&_17$$14, cachedContent, SL("content"), PH_NOISY | PH_READONLY, "phalcon/cache/backend/file.zep", 541 TSRMLS_CC);
				ZEPHIR_CPY_WRT(cachedContent, _17$$14);
			}
			if (ZEPHIR_IS_FALSE_IDENTICAL(cachedContent)) {
				ZEPHIR_INIT_VAR(_18$$15);
				object_init_ex(_18$$15, phalcon_cache_exception_ce);
				ZEPHIR_INIT_VAR(_19$$15);
				ZEPHIR_CONCAT_SVS(_19$$15, "Cache file ", cacheFile, " could not be opened");
				ZEPHIR_CALL_METHOD(NULL, _18$$15, "__construct", NULL, 9, _19$$15);
				zephir_check_call_status();
				zephir_throw_exception_debug(_18$$15, "phalcon/cache/backend/file.zep", 545 TSRMLS_CC);
				ZEPHIR_MM_RESTORE();
				return;
			}
			if (zephir_is_numeric(cachedContent)) {
				ZEPHIR_INIT_VAR(newValue);
				ZVAL_LONG(newValue, (zephir_get_numberval(cachedContent) - value));
				ZEPHIR_INIT_VAR(_20$$16);
				zephir_create_array(_20$$16, 3, 0 TSRMLS_CC);
				ZEPHIR_INIT_VAR(_21$$16);
				zephir_time(_21$$16);
				zephir_array_update_string(&_20$$16, SL("created"), &_21$$16, PH_COPY | PH_SEPARATE);
				ZEPHIR_INIT_NVAR(_21$$16);
				ZVAL_LONG(_21$$16, ttl);
				zephir_array_update_string(&_20$$16, SL("lifetime"), &_21$$16, PH_COPY | PH_SEPARATE);
				zephir_array_update_string(&_20$$16, SL("content"), &newValue, PH_COPY | PH_SEPARATE);
				ZEPHIR_INIT_VAR(result);
				zephir_json_encode(result, &(result), _20$$16, 0  TSRMLS_CC);
				ZEPHIR_INIT_NVAR(_21$$16);
				zephir_file_put_contents(_21$$16, cacheFile, result TSRMLS_CC);
				if (ZEPHIR_IS_FALSE_IDENTICAL(_21$$16)) {
					ZEPHIR_THROW_EXCEPTION_DEBUG_STR(phalcon_cache_exception_ce, "Cache directory can't be written", "phalcon/cache/backend/file.zep", 553);
					return;
				}
				RETURN_CCTOR(newValue);
			} else {
				ZEPHIR_THROW_EXCEPTION_DEBUG_STR(phalcon_cache_exception_ce, "The cache value is not numeric, therefore could not decrement it", "phalcon/cache/backend/file.zep", 558);
				return;
			}
		} else {
			ZEPHIR_CALL_METHOD(NULL, this_ptr, "delete", NULL, 0, keyName);
			zephir_check_call_status();
			RETURN_MM_NULL();
		}
	} else {
		ZEPHIR_INIT_VAR(_22$$20);
		object_init_ex(_22$$20, phalcon_cache_exception_ce);
		ZEPHIR_INIT_VAR(_23$$20);
		ZEPHIR_CONCAT_SVS(_23$$20, "Cache file ", cacheFile, " could not be opened");
		ZEPHIR_CALL_METHOD(NULL, _22$$20, "__construct", NULL, 9, _23$$20);
		zephir_check_call_status();
		zephir_throw_exception_debug(_22$$20, "phalcon/cache/backend/file.zep", 568 TSRMLS_CC);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_MM_RESTORE();

}

/**
 * Immediately invalidates all existing items.
 */
PHP_METHOD(Phalcon_Cache_Backend_File, flush) {

	zend_bool _4$$5;
	zend_object_iterator *_1;
	zval *prefix = NULL, *cacheDir = NULL, *item = NULL, *key = NULL, *cacheFile = NULL, *_0, *_2, *_3$$4 = NULL, *_5$$6 = NULL;
	zephir_fcall_cache_entry *_6 = NULL;
	int ZEPHIR_LAST_CALL_STATUS;

	ZEPHIR_MM_GROW();

	ZEPHIR_OBS_VAR(prefix);
	zephir_read_property_this(&prefix, this_ptr, SL("_prefix"), PH_NOISY_CC);
	ZEPHIR_OBS_VAR(cacheDir);
	_0 = zephir_fetch_nproperty_this(this_ptr, SL("_options"), PH_NOISY_CC);
	if (!(zephir_array_isset_string_fetch(&cacheDir, _0, SS("cacheDir"), 0 TSRMLS_CC))) {
		ZEPHIR_THROW_EXCEPTION_DEBUG_STR(phalcon_cache_exception_ce, "Unexpected inconsistency in options", "phalcon/cache/backend/file.zep", 582);
		return;
	}
	ZEPHIR_INIT_VAR(_2);
	object_init_ex(_2, spl_ce_DirectoryIterator);
	ZEPHIR_CALL_METHOD(NULL, _2, "__construct", NULL, 110, cacheDir);
	zephir_check_call_status();
	_1 = zephir_get_iterator(_2 TSRMLS_CC);
	_1->funcs->rewind(_1 TSRMLS_CC);
	for (;_1->funcs->valid(_1 TSRMLS_CC) == SUCCESS && !EG(exception); _1->funcs->move_forward(_1 TSRMLS_CC)) {
		{
			zval **ZEPHIR_TMP_ITERATOR_PTR;
			_1->funcs->get_current_data(_1, &ZEPHIR_TMP_ITERATOR_PTR TSRMLS_CC);
			ZEPHIR_CPY_WRT(item, (*ZEPHIR_TMP_ITERATOR_PTR));
		}
		ZEPHIR_CALL_METHOD(&_3$$4, item, "isfile", NULL, 0);
		zephir_check_call_status();
		if (likely(ZEPHIR_IS_TRUE(_3$$4))) {
			ZEPHIR_CALL_METHOD(&key, item, "getfilename", NULL, 0);
			zephir_check_call_status();
			ZEPHIR_CALL_METHOD(&cacheFile, item, "getpathname", NULL, 0);
			zephir_check_call_status();
			_4$$5 = ZEPHIR_IS_EMPTY(prefix);
			if (!(_4$$5)) {
				_4$$5 = zephir_start_with(key, prefix, NULL);
			}
			if (_4$$5) {
				ZEPHIR_CALL_FUNCTION(&_5$$6, "unlink", &_6, 109, cacheFile);
				zephir_check_call_status();
				if (!(zephir_is_true(_5$$6))) {
					RETURN_MM_BOOL(0);
				}
			}
		}
	}
	_1->funcs->dtor(_1 TSRMLS_CC);
	RETURN_MM_BOOL(1);

}

/**
 * Return a file-system safe identifier for a given key
 */
PHP_METHOD(Phalcon_Cache_Backend_File, getKey) {

	zval *key, *_0;

	zephir_fetch_params(0, 1, 0, &key);



	_0 = zephir_fetch_nproperty_this(this_ptr, SL("_useSafeKey"), PH_NOISY_CC);
	if (ZEPHIR_IS_TRUE_IDENTICAL(_0)) {
		zephir_md5(return_value, key);
		return;
	}
	RETVAL_ZVAL(key, 1, 0);
	return;

}

/**
 * Set whether to use the safekey or not
 *
 * @return this
 */
PHP_METHOD(Phalcon_Cache_Backend_File, useSafeKey) {

	zval *useSafeKey_param = NULL;
	zend_bool useSafeKey;

	zephir_fetch_params(0, 1, 0, &useSafeKey_param);

	useSafeKey = zephir_get_boolval(useSafeKey_param);


	if (useSafeKey) {
		zephir_update_property_this(this_ptr, SL("_useSafeKey"), ZEPHIR_GLOBAL(global_true) TSRMLS_CC);
	} else {
		zephir_update_property_this(this_ptr, SL("_useSafeKey"), ZEPHIR_GLOBAL(global_false) TSRMLS_CC);
	}
	RETURN_THISW();

}

