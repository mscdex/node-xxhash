/*global describe:true, it:true, before:true, after:true */

var demand = require('must');
var XXHash = require('../lib/xxhash');

var seed = 0xcafed00d;

describe('XXHash', function()
{
	it('requires a seed passed to its constructor', function()
	{
		function shouldThrow() { return new XXHash(); }
		shouldThrow.must.throw(/Expected unsigned integer seed argument/);
	});

	it('can be constructed', function()
	{
		var hasher = new XXHash(seed);
		hasher.must.be.an(XXHash);
	});

	it('has an update() method', function()
	{
		var hasher = new XXHash(seed);
		hasher.must.have.property('update');
		hasher.update.must.be.a.function();
	});

	it('update() requires a buffer argument', function()
	{
		function shouldThrow()
		{
			var hasher = new XXHash(seed);
			hasher.update('foo');
		}

		shouldThrow.must.throw(/data argument must be a Buffer/);
	});

	it('update() can be called repeatedly', function()
	{
		var hasher = new XXHash(seed);
		hasher.update(new Buffer('foo'));
		hasher.update(new Buffer('bar'));
	});

	it('has a digest() method', function()
	{
		var hasher = new XXHash(seed);
		hasher.must.have.property('digest');
		hasher.digest.must.be.a.function();
	});

	it('returns an integer from digest()', function()
	{
		var hasher = new XXHash(seed);
		hasher.update(new Buffer('foo'));
		var result = hasher.digest();
		result.must.be.a.number();
	});

	function hashHelper(input)
	{
		var hasher = new XXHash(seed);
		hasher.update(new Buffer(input));
		return hasher.digest();
	}

	it('hashes to the expected results', function()
	{
		var inputs = [ 'bar', 'coatis have long tails'];
		var results = [ 2370523005, 1792348589];

		for (var i = 0; i < inputs.length; i++)
			hashHelper(inputs[i]).must.equal(results[i]);
	});

	it('update() cannot be called after digest() has been called', function()
	{
		function shouldThrow()
		{
			var hasher = new XXHash(seed);
			hasher.update(new Buffer('foo'));
			hasher.digest();
			hasher.update(new Buffer('bar'));
		}

		shouldThrow.must.throw(/cannot call update after digest/);
	});
})
