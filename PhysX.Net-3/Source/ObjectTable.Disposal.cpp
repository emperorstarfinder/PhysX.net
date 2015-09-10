#include "StdAfx.h"
#include "ObjectTable.h"
#include "IDisposable.h"
#include "PhysXException.h"

using namespace System::Threading;
using namespace System::Linq;

void ObjectTable::disposableObject_OnDisposing(Object^ sender, EventArgs^ e)
{
	// In this method we build a complete list of objects that need disposal, then iterate through them.
	// Each disposal call will trigger the OnDisposing event which won't cause a crash, but is not needed
	// and will hurt performance.
	if (_performingDisposal)
		return;

	_performingDisposal = true;

		// Call Dispose on the object and its dependents
		DisposeOfObjectAndDependents(dynamic_cast<PhysX::IDisposable^>(sender));

		// Remove the object from all the dictionaries
		Remove(sender);

	_performingDisposal = false;
}

void ObjectTable::DisposeOfObjectAndDependents(PhysX::IDisposable^ disposable)
{
	if (disposable == nullptr || disposable->Disposed || !_ownership->ContainsKey(disposable))
		return;

	// Get all dependent objects of the current disposable object
	// Objects are returned in reverse depedent order - (Shape, Actor, Scene, Physics)
	auto dependents = GetDependents(disposable);

	// Dispose of the object's children first
	for each(PhysX::IDisposable^ dependent in dependents)
	{
		delete dependent;
	}
		
	// Dispose the object
	delete disposable;

	// Remove the object and its dependents from the various dictionaries
	for each(PhysX::IDisposable^ dependent in dependents)
	{
		Remove(dependent);
	}
}

List<PhysX::IDisposable^>^ ObjectTable::GetDependents(PhysX::IDisposable^ disposable)
{
	auto allDependents = gcnew List<PhysX::IDisposable^>();

	GetDependents(disposable, allDependents);

	return allDependents;
}

void ObjectTable::GetDependents(PhysX::IDisposable^ disposable, List<PhysX::IDisposable^>^ allDependents)
{
	OwnershipInfo^ ownInfo;
	if (_ownership->TryGetValue(disposable, ownInfo))
	{
		IEnumerable<PhysX::IDisposable^>^ deps = ownInfo->Owns;
		for each(PhysX::IDisposable^ d in deps)
		{
			// Recurse first before adding the object to make a reverse tree
			// e.g. Actor - Scene - Physics
			GetDependents(d, allDependents);

			allDependents->Add(d);
		}
	}

	
}