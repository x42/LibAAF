#ifndef __AAFCore_h__
#define __AAFCore_h__

/*
 *	This file is part of LibAAF.
 *
 *	Copyright (c) 2017 Adrien Gesta-Fline
 *
 *	LibAAF is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU Affero General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	any later version.
 *
 *	LibAAF is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU Affero General Public License for more details.
 *
 *	You should have received a copy of the GNU Affero General Public License
 *	along with LibAAF. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *	@file AAFCore/AAFCore.h
 *	@brief Retrieves the AAF Objects Tree from the Compound File Binary.
 *	@author Adrien Gesta-Fline
 *	@version 0.1
 *	@date 04 october 2017
 *
 *	@ingroup AAFCore
 *	@addtogroup AAFCore
 *	@brief Retrieves the AAF Objects Tree from the Compound File Binary.
 *
 *	The AAF file structure is based on the Compound File Binary, implemented by LibCFB
 *	and defined at https://www.amwa.tv/projects/MS-03.shtml
 *
 *	The specifications of the low-level AAF can be found at https://amwa.tv/projects/MS-02.shtml
 *
 *	The specifications of the overall AAF and standard Classes / Properties can be found at
 *	https://amwa.tv/projects/MS-01.shtml
 *
 *	This is the core of the libAAF library. AAFCore is intendend to :
		- Define the standard AAF classes and properties at run time,
		- retrieve potential custom classes and properties out of the MetaDictionary,
		- retrieve the entire object tree and their properties out of the CFB Tree,
		- provide functions to navigate in the tree and access objects and properties.

 *	Therefore, AAFCore makes a bridge between the low-level Compound File Binary and the
 *	high level AAFIface, which is used to interpret objects and thus retrieve usable data.
 *
 *	Even though AAFCore can be used as is, it is recommended for complex operations
 *	like essences and clips retrieval, to use AAFIface.
 *
 *	### Usage
 *
 *	In order to use libAAF, you should start by allocating AAF_Data with aaf_alloc(), then
 *	you can load a file by calling aaf_load_file(). aaf_load_file() will not load the entire
 *	file to memory, but will instead parse the CFB Tree and set AAF_Data accordingly.
 *
 *	@code
 *	AAF_Data *aafd = aaf_alloc();
 *	aaf_load_file( aafd, "./path/to/file.aaf" );
 *	@endcode
 *
 *	Then, you can access the object tree thanks to the access functions, or use the AAFIface
 *	to extract usable data through a convenient interface.
 *
 *	@code
 *	// TODO
 *	@endcode
 *
 *	Once you're done, you can close the file and free the AAF_Data structure by calling
 *	aaf_release( &aafd );
 *
 *	@code
 *	aaf_release( &aafd );
 *	@endcode
 *
 *	@{
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "AAFTypes.h"

#include "../LibCFB/LibCFB.h"







/**
 *	Possible values for aafPropertyDef.isReq.
 */
enum aafPropertyIsRequired_e
{
	REQUIRED = 1,
	REQ      = 1,
	OPTIONAL = 0,
	OPT      = 0
};

/**
 *	This structure defines a Class property, with a property ID
 *	and tells if that property is either #REQUIRED or #OPTIONAL.
 *	This structure is to be added to an aafClass.Properties list
 *	with the attachNewProperty() macro, and to be pointed to by
 *	the aafProperty.def field of each parsed property.
 */
typedef struct aafPropertyDefinition
{
	/**
	 *	Specifies the property ID.
	 *
	 *	All the standard IDs can be found in AAFDefs/AAFPropertyIDs.h.
	 */

	uint16_t      pid;


	/**
	 *	Tells if that property is either #REQUIRED or #OPTIONAL.
	 */

	aafBoolean_t  isReq;

	aafBoolean_t  meta;

	wchar_t      *name; // only when Meta PropEntry. TODO realy ?


	/**
	 *	Pointer to the next aafPropertyDef in the list.
	 */
	struct aafPropertyDefinition *next;

} aafPropertyDef;



/**
 *	Possible values for aafClass.isConcrete.
 */
enum aafClassIsConcrete_e
{
	ABSTRACT = 0,
	ABS      = 0,
	CONCRETE = 1,
	CON      = 1
};


/**
 *	This structure defines an AAF Class.
 *
 *	An AAF Class can be identified thanks to its ClassID.
 *
 *	An AAF Class holds some properties.
 *
 *	An AAF Class can inherit other Classes properties from
 *	its parents.
 *
 *	An AAF Class can be either a #CONCRETE or an #ABSTRACT
 *	Class. It is a #CONCRETE Class if it can be directly
 *	retrieved from the Compound File Nodes Tree as an object.
 *	Else, it is an #ABSTRACT Class and has to be inherited by
 *	another Class for its properties to be retrieved.
 */
typedef struct aafclass
{
	/**
	 *	The ClassID of the Class. When parsing, the ClassID
	 *	correspond to the cfbNode._clsId.
	 *	Note that the CLSID and aafUID_t types are binary
	 *	compatible.
	 *
	 *	All the standard ClassIDs can be found in AAFDefs/AAFClassDefUIDs.h.
	 */
	const aafUID_t    *ID;


	/**
	 *	Tells if the Class is either #CONCRETE or #ABSTRACT.
	 *
	 *	A Class is #CONCRETE if it can be retrieved as an
	 *	object. Else, a Class is #ABSTRACT if it can't be
	 *	directly retrieved and can only be inherited by
	 *	another Class, so only its memebers can be retrieved.
	 */

	aafBoolean_t       isConcrete;


	/**
	 *	Pointer to a list of aafPropertyDef structs.
	 *
	 *	These aafPropertyDef define the properties
	 *	owned by the Class.
	 */
	aafPropertyDef    *Properties;


	/**
	 *	Pointer to the Parent Class. If this Class has no
	 *	Parent (is root), then the pointer shall be NULL.
	 */

	struct aafclass  *Parent;


	aafBoolean_t      meta;


	wchar_t          *name; // this is set at runtime


	/**
	 *	Pointer to the next Class in the AAF_Data.Class list.
	 */
	struct aafclass  *next;

} aafClass;



/**
 *	This structure represents a property once it has been parsed.
 *
 *	This structure is to be added to an aafObject.Properties list.
 */

typedef struct aafProperty
{
	/**
	 *	Specifies the property ID. The pid shall be the same as in the
	 *	#def aafPropertyDef structure.
	 *
	 *	All the standard IDs can be found in AAFDefs/AAFPropertyIDs.h.
	 */

	uint16_t             pid;


	/**
	 *	The _storedForm identifies the “type” of representation chosen
	 *	for this property. This field describes how the property value
	 *	should be interpreted. Note that the stored form described here
	 *	is not the data type of the property value, rather it is the
	 *	type of external representation employed. The data type of a
	 *	given property value is implied by the property ID.
	 *
	 *	Can take one of the value from #aafStoredForm_e enum.
	 */

	uint16_t sf;

	/**
	 *	Holds a pointer to the corresponding property definition
	 *	aafPropertyDef struct.
	 */

	aafPropertyDef      *def;


	/**
	 *	The length, in bytes, of the #val property value.
	 */

	uint16_t             len;


	/**
	 *	The actual property value, of #len length.
	 */

	void                *val;


	/**
	 *	Pointer to the next property in an aafObject.Properties list.
	 */

	struct aafProperty *next;

} aafProperty;



/**
 *	This structure represents an AAF Object, once it has been parsed.
 *
 *	This structure is to be added to the AAF_Data.Objects list.
 *
 *	Each aafObject correspond to a Compound File Tree Node.
 *
 *	Each aafObject property correspond to a property entry in this
 *	Node/properties stream.
 */

typedef struct aafObject
{
	/**
	 *	Pointer to the corresponding Class, in the AAF_Data.Class list.
	 */

	aafClass     *Class;


	/**
	 *	Pointer to the corresponding Node in the Compound File Tree.
	 */

	cfbNode     *Node;


	/**
	 *	The name of the Node in the Compound File Tree : cfbNode._ab.
	 *
	 *	Note that the Compound File specifies that each string shall
	 *	use wide char UTF-16 encoding. So the #Name here has to be
	 *	converted to ascii, thanks to the utf16toa() function.
	 */

	wchar_t       Name[CFB_NODE_NAME_SZ];
	// wchar_t      *Name;


	/**
	 *	Pointer to an aafProperty list. This list holds the retrieved
	 *	Object properties.
	 */

	aafProperty  *Properties;


	/**
	 *	Pointer to an aafStrongRefSetHeader_t struct.
	 *
	 *	This pointer keeps track of the Index Header, when the
	 *	Object belongs to either a Set or a Vector, else, it shall
	 *	remain NULL.
	 *
	 *	Here we can use an aafStrongRefSetHeader_t struct to hold
	 *	an aafStrongRefVectorHeader_t, because both structs begin
	 *	with the same bytes, exept the first one is bigger. So in
	 *	case of a Vector, the remaining bytes will simply remain
	 *	NULL.
	 */

	aafStrongRefSetHeader_t *Header;


	/**
	 *	Pointer to an aafStrongRefSetEntry_t struct.
	 *
	 *	This pointer keeps track of the Index Entry, when the
	 *	Object belongs to either a Set or a Vector, else, it shall
	 *	remain NULL.
	 *
	 *	Here we can use an aafStrongRefSetEntry_t struct to hold
	 *	an aafStrongRefVectorEntry_t, because both structs begin
	 *	with the same bytes, exept the first one is bigger. So in
	 *	case of a Vector, the remaining bytes will simply remain
	 *	NULL.
	 */

	aafStrongRefSetEntry_t  *Entry;


	/**
	 *	Pointer to the Parent Object, that is the upper "Node" in
	 *	the Compound File Tree.
	 */

	struct aafObject        *Parent;


	/**
	 *	Pointer to the next Object in Set/Vector
	 */

	struct aafObject        *next;

	struct aafObject        *prev;


	/**
	 *	Pointer to the next Object in the AAF_Data.Objects list.
	 */

	struct aafObject        *nextObj;


	struct _aafData         *aafd; // only to access aafd->verb

} aafObject;




/**
 *	This structure is the main structure when using LibAAF.
 *
 *	It holds a pointer to the CFB_Data structure for file
 *	access, a pointer to the AAF Classes list, a pointer to
 *	the AAF Objects list and a bunch of pointers to some
 *	key Objects in the AAF Objects Tree called shortcuts.
 *
 *	Those shortcuts allows to quickly retrieve some AAF
 *	key Objects without the need to parse the tree everytime.
 */

typedef struct _aafData
{
	/**
	 *	Pointer to the LibCFB CFB_Data structure.
	 */

	CFB_Data   *cfbd;


	/**
	 *	Pointer to the AAF Classes list.
	 */

	aafClass   *Classes;


	/**
	 *	Pointer to the AAF Object list.
	 *
	 *	@note This list is intended to keep track of all the allocated Objects, not for
	 *	      parsing. For tree access, the AAF_Data.Root pointer should be used.
	 */

	aafObject  *Objects;


	struct Header {

		aafObject *obj;

		int16_t ByteOrder;
		aafTimeStamp_t *LastModified;
		aafVersionType_t *Version;
		uint32_t ObjectModelVersion;
		aafUID_t *OperationalPattern;
		// EssenceContainers; TODO AUIDSet_t
		// DescriptiveSchemes: TODO AUIDSet_t

	} Header;


	struct Identification {

		aafObject            *obj;

		wchar_t              *CompanyName;
		wchar_t              *ProductName;
		aafProductVersion_t  *ProductVersion;
		wchar_t              *ProductVersionString;
		aafUID_t             *ProductID;
		aafTimeStamp_t       *Date;
		aafProductVersion_t  *ToolkitVersion;
		wchar_t              *Platform;
		aafUID_t             *GenerationAUID;

	} Identification;


	/**
	 *	Pointer to the first Root Object, that is to the the top of the Tree.
	 */

	aafObject  *Root;


	/**
	 *	(Shortcut) pointer to the Header Object in the Tree.
	 */

	// aafObject  *Header;


	/**
	 *	(Shortcut) pointer to the MetaDictionary Object in the Tree.
	 */

	aafObject  *MetaDictionary;


	/**
	 *	(Shortcut) pointer to the ClassDefinition Object in the Tree.
	 */

	aafObject  *ClassDefinition;


	/**
	 *	(Shortcut) pointer to the TypeDefinition Object in the Tree.
	 */

	aafObject  *TypeDefinition;


	/**
	 *	(Shortcut) pointer to the Identification Object in the Tree.
	 */

	// aafObject  *Identification;


	/**
	 *	(Shortcut) pointer to the Content Object in the Tree.
	 */

	aafObject  *Content;


	/**
	 *	(Shortcut) pointer to the Dictionary Object in the Tree.
	 */

	aafObject  *Dictionary;


	/**
	 *	(Shortcut) pointer to the Mobs Object in the Tree.
	 */

	aafObject  *Mobs;


	/**
	 *	(Shortcut) pointer to the EssenceData Object in the Tree.
	 */

	aafObject  *EssenceData;


	/**
	 *	(Shortcut) pointer to the OperationDefinition Object in the Tree.
	 */

	aafObject  *OperationDefinition;


	/**
	 *	(Shortcut) pointer to the ParameterDefinition Object in the Tree.
	 */

	aafObject  *ParameterDefinition;


	/**
	 *	(Shortcut) pointer to the DataDefinition Object in the Tree.
	 */

	aafObject  *DataDefinition;


	/**
	 *	(Shortcut) pointer to the PluginDefinition Object in the Tree.
	 */

	aafObject  *PluginDefinition;


	/**
	 *	(Shortcut) pointer to the CodecDefinition Object in the Tree.
	 */

	aafObject  *CodecDefinition;


	/**
	 *	(Shortcut) pointer to the ContainerDefinition Object in the Tree.
	 */

	aafObject  *ContainerDefinition;


	/**
	 *	(Shortcut) pointer to the InterpolationDefinition Object in the Tree.
	 */

	aafObject  *InterpolationDefinition;


	/**
	 *	(Shortcut) pointer to the KLVDataDefinition Object in the Tree.
	 */

	aafObject  *KLVDataDefinition;


	/**
	 *	(Shortcut) pointer to the TaggedValueDefinition Object in the Tree.
	 */

	aafObject  *TaggedValueDefinition;


	verbosityLevel_e verb;

} AAF_Data;






/**
 *	@name Initialisation functions
 *	@{
 */

/**
 *	Allocates a new AAF_Data structure.
 *
 *	@return  A pointer to the newly allocated structure.
 */

AAF_Data * aaf_alloc();


/**
 *	Loads an AAF file and sets the AAF_Data sructure accordingly.
 *
 *	@param  aafd  Pointer to the AAF_Data structure.
 *	@param  file  Pointer to a null terminated string holding the filepath.
 *
 *	@return       0 on success\n
 *	              1 on failure
 */

int aaf_load_file( AAF_Data *aafd, const char *file );

/**
 *	@}
 */



/**
 *	@name Release function
 *	@{
 */

/**
 *	Releases the CFB_Data structure, fclose() the file and frees the AAF_Data structure.
 *
 *	@param  aafd  Pointer to the AAF_Data structure.
 */

void aaf_release( AAF_Data **aafd );


/**
 *	@}
 */



/**
 *	@name Access functions
 *	@{
 */

/**
 *	Retrieves, for a given Object, its path in the Compound File Binary Tree.
 *
 *	@note It is the caller responsability to free the returned pointer.
 *
 *	@param  Obj  Pointer to the aafObject.
 *
 *	@return      Pointer to a null-terminated string holding the Object's path.
 */

wchar_t * aaf_get_ObjectPath( aafObject *Obj );


/**
 *	Resolves a given WeakReference from a given Dictionary (a Set or Vector list of
 *	aafObjects), and returns a pointer to the corresponding Object if it was found.
 *
 *	@param ref  Pointer to the WeakReference to search for.
 *	@param list Pointer to the aafObject list from which the Reference will be searched for.
 *
 *	@return     A pointer to the aafObject if found,\n
 *	            NULL otherwise.
 *
 */

aafObject * aaf_get_ObjectByWeakRef( aafObject    *list,
                                     aafWeakRef_t *ref );


/**
 *	Retrieves a Mob Object by its given MobID.
 *
 *	@param  Mobs  Pointer to the Mob Object list.
 *	@param  MobID Pointer to the MobID where're looking for.
 *
 *	@return       A pointer to the Mob aafObject if found,\n
 *	              NULL otherwise.
 */

aafObject * aaf_get_MobByID( aafObject  *Mobs,
                             aafMobID_t *MobID );



aafObject * aaf_get_MobSlotBySlotID( aafObject *MobSlots,
                                     aafSlotID_t SlotID );


/**
 *	Loops through each aafObject of a list, that is of a Set or Vector. It is also
 *	possible to filter the returned Object by ClassID.
 *
 *	This function should be used as the conditional expression of a while loop. The
 *	aaf_foreach_ObjectInSet() is an implementation of this.
 *
 *	@param  Obj    Pointer to pointer to the aafObject structure that will receive each
 *	               Object of the list.
 *	@param  head   Pointer to the first Object of the list.
 *	@param  filter Pointer to a ClassID to use as a filter, shall be NULL if unused.
 *
 *	@return        1 if has another Object,\n
 *	               0 if has no more Object.
 */

int aaf__foreach_ObjectInSet( aafObject     **Obj,
                              aafObject      *head,
                              const aafUID_t *filter );

/**
 *	Convenience macro, that implements the aaf__foreach_ObjectInSet() function. This should
 *	be used istead of directly calling aaf__foreach_ObjectInSet().
 */

#define aaf_foreach_ObjectInSet( Obj, head, filter ) \
	while ( aaf__foreach_ObjectInSet( Obj, head, filter ) )


/**
 *	Retrieves a Property by its ID, out of an Object.
 *
 *	@param  Obj  Pointer to the Object to get the property from.
 *	@param  pid  Index of the requested property.
 *
 *	@return      A pointer to the property if found,\n
 *	             NULL otherwise.
 */

aafProperty * aaf_get_property( aafObject *Obj,
                                aafPID_t   pid );


/**
 *	Retrieves a Property ID by its name.
 *
 *	@param  aafd  Pointer to the AAF_Data structure.
 *	@param  name  Name of the property to look for.
 *
 *	@return      The PID of the property if it was found\n
 *	             0 otherwise.
 */

aafPID_t aaf_get_PropertyIDByName( AAF_Data      *aafd,
                                   const wchar_t *name );


/**
 *	Retrieves a Property by its ID out of an Object, and returns its value.
 *
 *	@param  Obj  Pointer to the Object to get the property from.
 *	@param  pid  Index of the requested property.
 *
 *	@return      A pointer to the property's value if found,\n
 *	             NULL otherwise.
 */

void * aaf_get_propertyValue( aafObject *Obj,
                              aafPID_t   pid );


/**
 *	Retrieves a "text" Property by its ID out of an Object, handles the conversion from
 *	UTF-16 to ascii of the value and returns it.
 *
 *	@note It is the caller responsability to free the returned pointer.
 *
 *	@param  Obj  Pointer to the Object to get the property from.
 *	@param  pid  Index of the requested property.
 *
 *	@return      A pointer to the property's text value if found,\n
 *	             NULL otherwise.
 */

// char * aaf_get_propertyValueText( aafObject *Obj,
//                                   aafPID_t   pid );


wchar_t * aaf_get_propertyValueWstr( aafObject *Obj, aafPID_t pid );


/**
 *	Retrieves a Property by its ID out of an Object, interprets it as an aafIndirect_t and
 *	returns the Indirect value.
 *
 *	@param  Obj  Pointer to the Object to get the property from.
 *	@param  pid  Index of the requested property.
 *
 *	@return      A pointer to the property's Indirect value if found,\n
 *	             NULL otherwise.
 */

void * aaf_get_propertyIndirectValue( aafObject *Obj,
                                      aafPID_t   pid );




wchar_t * aaf_get_propertyIndirectValueWstr( aafObject *Obj, aafPID_t pid );


/**
 *	@}
 */



/**
 *	@}
 */

#endif // ! __AAFCore_h__
