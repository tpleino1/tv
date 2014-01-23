#ifndef MB_RESOURCECONTAINER_HH
#define MB_RESOURCECONTAINER_HH

#include <map>			// map
#include <memory>		// unique_ptr, move
#include <string>		// string
#include <stdexcept>	// runtime_error, throw
#include <cassert>		// assert
#include <utility>		// make_pair

// General-purpose container for various resource types
// Not to be used with music (which is based on streaming)
template <typename Res, typename Id>
class ResourceContainer
{
	public:
		ResourceContainer();	// Constructor

		void load(Id id, const std::string& fn); 	// Add resource (from file)
		void load(Id id, const Res& s); 			// Add resource (via copy)

		Res& get(Id id);				// Get resource
		const Res& get(Id id) const;	// Get const resource

	private:
		std::map<Id, std::unique_ptr<Res>> container;	// Container for the unique resources
};

// Constructor
template <typename Res, typename Id>
ResourceContainer<Res, Id>::ResourceContainer() : container()
{ }

// Add resource (from file)
template <typename Res, typename Id>
void ResourceContainer<Res, Id>::load(Id id, const std::string& fn)
{
	assert(container.find(id) == container.end()); // Programming error elsewhere
	std::unique_ptr<Res> r(new Res());
	if (!r->loadFromFile(fn))			// Could not be load
		throw std::runtime_error("Resource could not be load from '" + fn + "'.");

	auto ptr = container.insert(std::make_pair(id, std::move(r)));	// Take control of the pointer
	assert(ptr.second); // Just to make sure
}

// Add resource (via copy)
template <typename Res, typename Id>
void ResourceContainer<Res, Id>::load(Id id, const Res& s)
{
	assert(container.find(id) == container.end()); // Programming error elsewhere
	std::unique_ptr<Res> r(new Res(s));

	auto ptr = container.insert(std::make_pair(id, std::move(r)));	// Take control of the pointer
	assert(ptr.second); // Just to make sure
}

// Get resource
template <typename Res, typename Id>
Res& ResourceContainer<Res, Id>::get(Id id)
{
	auto ptr = container.find(id);
	if (ptr == container.end())
		throw std::logic_error("Resource '" + id + "' not initialized.");
	//assert(ptr != container.end());	// Programming error elsewhere

	return *ptr->second;
}

// Get const resource
template <typename Res, typename Id>
const Res& ResourceContainer<Res, Id>::get(Id id) const
{
	auto ptr = container.find(id);
	if (ptr == container.end())
		throw std::logic_error("Resource '" + id + "' not initialized.");
	//assert(ptr != container.end());	// Programming error elsewhere

	return *ptr->second;
}

#endif // MB_RESOURCECONTAINER_HH
