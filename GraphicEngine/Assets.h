#pragma once

#include <map>
#include <Windows.h>
#include <d3d11.h>
#include <vector>
#include "Util.h"

#define ASSETS_HOTLOAD_DELAY 0.5f
#define ASSETS_MAX_UNLOAD_PER_FRAME 5


	class FileInfo
	{
	public:
		FileInfo();
		~FileInfo();

		GRAPHIC_API bool hasChanged();

		GRAPHIC_API void setPath( std::string& path );
		GRAPHIC_API const std::string& getPath() const;
		
	private:
		uint64_t getWriteTime();

		std::string path;
		uint64_t timestamp;
	};

	class Assets;
	class Asset
	{
	public:
		Asset();
		virtual ~Asset();

		virtual bool load( std::string path, Assets* assets ) = 0;
		virtual void unload() = 0;
		virtual void upload() = 0;

		GRAPHIC_API virtual void incrementReferenceCount();
		GRAPHIC_API virtual void decrementReferenceCount();

		GRAPHIC_API void setAssets( Assets* assets );

		GRAPHIC_API FileInfo* getFileInfo();
		GRAPHIC_API int getReferenceCount();

	protected:
		Assets* assets;
		int referenceCount;

	private:
		FileInfo fileInfo;
	};

	class AssetID
	{
	public:
		GRAPHIC_API AssetID( std::string path, size_t hash );
		GRAPHIC_API ~AssetID();

		GRAPHIC_API AssetID& operator=( const AssetID& ref );
		GRAPHIC_API bool operator==( const AssetID& ref ) const;
		GRAPHIC_API bool operator!=( const AssetID& ref ) const;
		GRAPHIC_API bool operator<( const AssetID& ref ) const;
		GRAPHIC_API bool operator>( const AssetID& ref ) const;

	private:
		std::string path;
		size_t hash;
	};

	class Assets
	{
	public:
		GRAPHIC_API Assets();
		GRAPHIC_API virtual ~Assets();

		template<typename T>
		T* load( std::string path )
		{
			T* result = nullptr;

			// NOTE: Since hash_code is a hash, it is not guaranteed to be unique.
			// But it should be good enough for our limited purposes
			AssetID id( path, typeid(T).hash_code() );

			std::map<AssetID, Asset*>::iterator it = assets.find( id );
			if( it != assets.end() )
				result = (T*)it->second;
			else
			{
				result = new T();
				if( result->load( path, this ) )
				{
					result->getFileInfo()->setPath( path );
					assets.insert( std::pair<AssetID, Asset*>( id, result ) );
					pending.push_back( result );
				}
				else
				{
					//printf( "Failed to load asset \"%s\"\n", path.c_str() );
					delete result;
					result = nullptr;
				}
			}

			if( result )
			{
				result->setAssets( this );
				result->incrementReferenceCount();
			}

			return result;
		}

		template<typename T>
		void unload( std::string path )
		{
			AssetID id( path, typeid(T).hash_code() );

			/*std::map<AssetID, Asset*>::iterator it = assets.find( id );
			if( it != assets.end() )
			{
				it->second->decrementReferenceCount();
			}*/

			unloads.push_back( id );
		}

		void bind(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
		{
			m_device = device;
			m_deviceContext = deviceContext;
		}

		GRAPHIC_API void upload();

		GRAPHIC_API void checkHotload( float dt );
		GRAPHIC_API void checkReferences();

		GRAPHIC_API const std::map<AssetID, Asset*>& getAssets() const;

		ID3D11Device* GetDevice() {
			return m_device;
		}
		ID3D11DeviceContext* GetDeviceContext() {
			return m_deviceContext;
		}

	private:
		float elapsedTime;
		std::map<AssetID, Asset*> assets;
		std::vector<Asset*> pending;
		std::vector<AssetID> unloads;
		std::vector<std::map<AssetID, Asset*>::iterator> removes;

		ID3D11Device* m_device;
		ID3D11DeviceContext* m_deviceContext;
	};
