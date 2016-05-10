
/*
 +------------------------------------------------------------------------+
 | Phalcon Framework	                                                  |
 +------------------------------------------------------------------------+
 | Copyright (c) 2011-2015 Phalcon Team (http://www.phalconphp.com)       |
 +------------------------------------------------------------------------+
 | This source file is subject to the New BSD License that is bundled     |
 | with this package in the file docs/LICENSE.txt.                        |
 |                                                                        |
 | If you did not receive a copy of the license and are unable to         |
 | obtain it through the world-wide-web, please send an email             |
 | to license@phalconphp.com so we can send you a copy immediately.       |
 +------------------------------------------------------------------------+
 | Authors: Andres Gutierrez <andres@phalconphp.com>                      |
 |          Eduar Carvajal <eduar@phalconphp.com>                         |
 +------------------------------------------------------------------------+
 */

namespace Phalcon\Cache\Backend;

use Phalcon\Cache\Exception;
use Phalcon\Cache\Backend;
use Phalcon\Cache\FrontendInterface;
use Phalcon\Cache\BackendInterface;

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
class File extends Backend implements BackendInterface
{
	/**
	 * Default to false for backwards compatibility
	 *
	 * @var boolean
	 */
	private _useSafeKey = false;

	/**
	 * Phalcon\Cache\Backend\File constructor
	 *
	 * @param	Phalcon\Cache\FrontendInterface frontend
	 * @param	array options
	 */
	public function __construct(<FrontendInterface> frontend, options = null)
	{
		var prefix, safekey;

		if !isset options["cacheDir"] {
			throw new Exception("Cache directory must be specified with the option cacheDir");
		}

		if fetch safekey, options["safekey"] {
			if typeof safekey !== "boolean" {
				throw new Exception("safekey option should be a boolean.");
			}

			let this->_useSafeKey = safekey;
		}

		// added to avoid having unsafe filesystem characters in the prefix
		if fetch prefix, options["prefix"] {
			if this->_useSafeKey && preg_match("/[^a-zA-Z0-9_.-]+/", prefix) {
				throw new Exception("FileCache prefix should only use alphanumeric characters.");
			}
		}

		parent::__construct(frontend, options);
	}

	/**
	 * Returns a cached content
	 *
	 * @param int|string keyName
	 * @param   int lifetime
	 * @return  mixed
	 */
	public function get(var keyName, lifetime = null)
	{
		var prefixedKey, cacheDir, cacheFile, frontend, lastLifetime, cachedContent, ret;
		int createdTime, ttl;

		if lifetime != null && lifetime < 1 {
			throw new Exception("The lifetime must be at least 1 second");
		}

		let prefixedKey =  this->_prefix . this->getKey(keyName);
		let this->_lastKey = prefixedKey;

		if !fetch cacheDir, this->_options["cacheDir"] {
			throw new Exception("Unexpected inconsistency in options");
		}

		let cacheFile = cacheDir . prefixedKey;

		if file_exists(cacheFile) == true {

			let frontend = this->_frontend;

			let cachedContent = json_decode(file_get_contents(cacheFile), true);
			/**
			 * Take the lifetime from the frontend or read it from the set in start()
			 * if the cachedContent is not a valid json array
			 */
			if !lifetime {
				if this->isValidArray(cachedContent, "lifetime") {
					let ttl = (int) cachedContent["lifetime"];
				} else {
					let lastLifetime = this->_lastLifetime;
					if !lastLifetime {
						let ttl = (int) frontend->getLifeTime();
					} else {
						let ttl = (int) lastLifetime;
					}
				}
			} else {
				let ttl = (int) lifetime;
			}
			if !this->isValidArray(cachedContent, "created") {
				let createdTime = (int) filemtime(cacheFile);
			} else {
				let createdTime = (int) cachedContent["created"];
			}

			/**
			 * Check if the file has expired
			 * The content is only retrieved if the content has not expired
			 */
			if !(time() - ttl > createdTime) {

				/**
				 * Use file-get-contents to control that the openbase_dir can't be skipped
				 */
				if !this->isValidArray(cachedContent, "content") {
					let cachedContent = file_get_contents(cacheFile);
				} else {
					let cachedContent = cachedContent["content"];
				}
				if cachedContent === false {
					throw new Exception("Cache file ". cacheFile. " could not be opened");
				}

				if is_numeric(cachedContent) {
					return cachedContent;
				} else {
					/**
					 * Use the frontend to process the content of the cache
					 */
					let ret = frontend->afterRetrieve(cachedContent);
					return ret;
				}
			} else {
				/**
				 * As the content is expired, deleting the cache file
				 */
				this->delete(keyName);
				return null;
			}
		} else {
			return null;
		}
	}

	/**
	 * Stores cached content into the file backend and stops the frontend
	 *
	 * @param int|string keyName
	 * @param string content
	 * @param int lifetime
	 * @param boolean stopBuffer
	 */
	public function save(var keyName = null, var content = null, lifetime = null, boolean stopBuffer = true) -> void
	{
		var lastKey, frontend, cacheDir, isBuffering, cacheFile, cachedContent, preparedContent, status,
			finalContent, lastLifetime;
		int ttl;

		if lifetime != null && lifetime < 1 {
			throw new Exception("The lifetime must be at least 1 second");
		}

		if keyName === null {
			let lastKey = this->_lastKey;
		} else {
			let lastKey = this->_prefix . this->getKey(keyName);
		}

		if !lastKey {
			throw new Exception("Cache must be started first");
		}

		let frontend = this->_frontend;

		if !fetch cacheDir, this->_options["cacheDir"] {
			throw new Exception("Unexpected inconsistency in options");
		}

		let cacheFile = cacheDir . lastKey;

		if content === null {
			let cachedContent = frontend->getContent();
		} else {
			let cachedContent = content;
		}

		let preparedContent = frontend->beforeStore(cachedContent);
		if !lifetime {
			 let lastLifetime = this->_lastLifetime;
			 if !lastLifetime {
				 let ttl = (int) frontend->getLifeTime();
			 } else {
				let ttl = (int) lastLifetime;
			}
		} else {
			let ttl = (int) lifetime;
		}
		/**
		 * We use file_put_contents to respect open-base-dir directive
		 */
		if !is_numeric(cachedContent) {
			let finalContent = json_encode(["created": time(), "lifetime": ttl, "content": preparedContent]);
		} else {
			let finalContent = json_encode(["created": time(), "lifetime": ttl, "content": cachedContent]);
		}
		let status = file_put_contents(cacheFile, finalContent);

		if status === false {
			throw new Exception("Cache file ". cacheFile . " could not be written");
		}

		let isBuffering = frontend->isBuffering();

		if stopBuffer === true {
			frontend->stop();
		}

		if isBuffering === true {
			echo cachedContent;
		}

		let this->_started = false;
	}

	/**
	 * Deletes a value from the cache by its key
	 *
	 * @param int|string keyName
	 * @return boolean
	 */
	public function delete(var keyName) -> boolean
	{
		var cacheFile, cacheDir;

		if !fetch cacheDir, this->_options["cacheDir"] {
			throw new Exception("Unexpected inconsistency in options");
		}

		let cacheFile = cacheDir . this->_prefix . this->getKey(keyName);
		if file_exists(cacheFile) {
			return unlink(cacheFile);
		}

		return false;
	}

	/**
	 * Query the existing cached keys
	 *
	 * @param string|int prefix
	 * @return array
	 */
	public function queryKeys(var prefix = null) -> array
	{
		var item, key, cacheDir;
		array keys = [];

		if !fetch cacheDir, this->_options["cacheDir"] {
			throw new Exception("Unexpected inconsistency in options");
		}

		/**
		 * We use a directory iterator to traverse the cache dir directory
		 */
		for item in iterator(new \DirectoryIterator(cacheDir)) {

			if likely item->isDir() === false {
				let key = item->getFileName();
				if prefix !== null {
					if starts_with(key, prefix) {
						let keys[] = key;
					}
				} else {
					let keys[] = key;
				}
			}
		}

		return keys;
	}

	/**
	 * Checks if cache exists and it isn't expired
	 *
	 * @param string|int keyName
	 * @param   int lifetime
	 * @return boolean
	 */
	public function exists(var keyName = null, int lifetime = null) -> boolean
	{
		var lastKey, prefix, cacheFile, cachedContent;
		int ttl;
		bool cacheFileExists;

		if !keyName {
			let lastKey = this->_lastKey;
		} else {
			let prefix = this->_prefix;
			let lastKey = prefix . this->getKey(keyName);
		}

		if lastKey {

			let cacheFile = this->_options["cacheDir"] . lastKey;
			let cacheFileExists = file_exists(cacheFile);
			if cacheFileExists {
				/**
				 * Check if the file has expired
				 */
				let cachedContent = json_decode(file_get_contents(cacheFile), true);
				if !lifetime {
			   		if this->isValidArray(cachedContent, "lifetime") {
						let ttl = (int) cachedContent["lifetime"];
					} else {
						let ttl = (int) this->_frontend->getLifeTime();
					}
				} else {
					let ttl = (int) lifetime;
				}

				if !this->isValidArray(cachedContent, "created") && filemtime(cacheFile) + ttl > time() {
					return true;
				} else {
					if (cachedContent["created"] + ttl > time()) {
						return true;
					}
				}
			}
		}

		/**
		 * If the cache file exists and is expired, delete it
		 */
		if cacheFileExists {
			this->delete(keyName);
		}
		return false;
	}

	/**
	 * Check if given variable is array, containing the key $cacheKey
	 *
	 * @param array|null cachedContent
	 * @param string|null cacheKey
	 * @return bool
	 */
	private function isValidArray(var cachedContent = null, var cacheKey = null)
	{
		return (is_array(cachedContent) && array_key_exists(cacheKey, cachedContent));
	}

	/**
	 * Increment of a given key, by number $value
	 *
	 * @param  string|int keyName
	 * @param  int value
	 * @return mixed
	 */
	public function increment(var keyName = null, int value = 1)
	{
		var prefixedKey, cacheFile, frontend, cachedContent, result, lastLifetime, newValue;
		int modifiedTime, ttl;

		let prefixedKey = this->_prefix . this->getKey(keyName),
			this->_lastKey = prefixedKey,
			cacheFile = this->_options["cacheDir"] . prefixedKey;

		if file_exists(cacheFile) {

			let frontend = this->_frontend;

			/**
			 * Check if the file has expired
			 */
			let cachedContent = json_decode(file_get_contents(cacheFile), true);
			/**
			 * Take the lifetime from the frontend or read it from the set in start()
			 * if the cachedContent is not a valid array
			 */

			if this->isValidArray(cachedContent, "lifetime") {
				let ttl = (int) cachedContent["lifetime"];
			} else {
				let lastLifetime = this->_lastLifetime;
				if !lastLifetime {
					let ttl = (int) frontend->getLifeTime();
				} else {
					let ttl = (int) lastLifetime;
				}
			}

			if !this->isValidArray(cachedContent, "created") {
				let modifiedTime = (int) filemtime(cacheFile);
			} else {
				let modifiedTime = (int) cachedContent["created"];
			}

			/**
			 * The content is only retrieved if the content has not expired
			 */
			if !(time() - ttl > modifiedTime) {

				/**
				 * Use file-get-contents to control that the openbase_dir can't be skipped
				 */
				if !this->isValidArray(cachedContent, "content") {
					let cachedContent = file_get_contents(cacheFile);
				} else {
					let cachedContent = cachedContent["content"];
				}

				if cachedContent === false {
					throw new Exception("Cache file " . cacheFile . " could not be opened");
				}

				if is_numeric(cachedContent) {
					let newValue = cachedContent + value;
					let result = json_encode(["created": time(), "lifetime": ttl, "content": newValue]);
					if file_put_contents(cacheFile, result) === false {
						throw new Exception("Cache directory could not be written");
					}

					return newValue;
				} else {
					throw new Exception("The cache value is not numeric, therefore could not be incremented");
				}
			} else {
				/**
				 * The cache file is expired, so we're removing it
				 */
				this->delete(keyName);
				return null;
			}
		}
		return null;
	}

	/**
	 * Decrement of a given key, by number $value
	 *
	 * @param  string|int keyName
	 * @param  int value
	 * @return mixed
	 */
	public function decrement(var keyName = null, int value = 1)
	{
		var prefixedKey, cacheFile, cachedContent, result, lastLifetime, newValue;
		int ttl, modifiedTime, lifetime;

		let prefixedKey = this->_prefix . this->getKey(keyName),
			this->_lastKey = prefixedKey,
			cacheFile = this->_options["cacheDir"] . prefixedKey;

		if file_exists(cacheFile) {

			/**
			 * Check if the file has expired
			 */
			let cachedContent = json_decode(file_get_contents(cacheFile), true);
			/**
			 * Take the lifetime from the frontend or read it from the set in start()
			 * if the cachedContent is not a valid array
			 */
			if !lifetime {
				if this->isValidArray(cachedContent, "lifetime") {
					let ttl = (int) cachedContent["lifetime"];
				} else {
					let lastLifetime = this->_lastLifetime;
					if !lastLifetime {
						let ttl = (int) this->_frontend->getLifeTime();
					} else {
						let ttl = (int) lastLifetime;
					}
				}
			} else {
				let ttl = (int) lifetime;
			}
			if !this->isValidArray(cachedContent, "created") {
				let modifiedTime = (int) filemtime(cacheFile);
			} else {
				let modifiedTime = (int) cachedContent["created"];
			}

			/**
			 * The content is only retrieved if the content has not expired
			 */
			if !(time() - ttl > modifiedTime) {

				/**
				 * Use file-get-contents to control that the openbase_dir can't be skipped
				 */
				if !this->isValidArray(cachedContent, "content") {
					let cachedContent = file_get_contents(cacheFile);
				} else {
					let cachedContent = cachedContent["content"];
				}

				if cachedContent === false {
					throw new Exception("Cache file " . cacheFile . " could not be opened");
				}

				if is_numeric(cachedContent) {

					let newValue = cachedContent - value;
					let result = json_encode(["created": time(), "lifetime": ttl, "content": newValue]);
					if file_put_contents(cacheFile, result) === false {
						throw new Exception("Cache directory can't be written");
					}

					return newValue;
				} else {
					throw new Exception("The cache value is not numeric, therefore could not decrement it");
				}
			} else {
				/**
				 * The cache file is expired, so we're removing it
				 */
				this->delete(keyName);
				return null;
			}
		}
		return null;
	}

	/**
	 * Immediately invalidates all existing items.
	 */
	public function flush() -> boolean
	{
		var prefix, cacheDir, item, key, cacheFile;

		let prefix = this->_prefix;

		if !fetch cacheDir, this->_options["cacheDir"] {
			throw new Exception("Unexpected inconsistency in options");
		}

		for item in iterator(new \DirectoryIterator(cacheDir)) {

			if likely item->isFile() == true {
				let key = item->getFileName(),
					cacheFile = item->getPathName();

				if empty prefix || starts_with(key, prefix) {
					if  !unlink(cacheFile) {
						return false;
					}
				}
			}
		}

		return true;
	}

	/**
	 * Return a file-system safe identifier for a given key
	 */
	public function getKey(key) -> string
	{
		if this->_useSafeKey === true {
			return md5(key);
		}

		return key;
	}

	/**
	 * Set whether to use the safekey or not
	 *
	 * @return this
	 */
	public function useSafeKey(bool useSafeKey)
	{
		let this->_useSafeKey = useSafeKey;

		return this;
	}
}
