#pragma once

#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <type_traits>
#include <cassert>
#include <memory>
#include <functional>

#include "textio.h"

namespace libply
{
  enum class Type
  {
    INT8,
    UINT8,
    INT16,
    UINT16,
    INT32,
    UINT32,
    FLOAT32,
    FLOAT64
  };

  class IProperty
  {
    public:
      virtual ~IProperty() = default;

      virtual IProperty &operator=( unsigned int value ) = 0;
      virtual IProperty &operator=( int value ) = 0;
      virtual IProperty &operator=( float value ) = 0;
      virtual IProperty &operator=( double value ) = 0;

      virtual operator unsigned int() = 0;
      virtual operator int() = 0;
      virtual operator float() = 0;
      virtual operator double() = 0;

  };

  template<typename InternalType>
  class ScalarProperty: public IProperty
  {
    public :

      virtual ~ScalarProperty() = default;

      virtual ScalarProperty &operator=( unsigned int value ) override
      { m_value = static_cast<InternalType>( value ); return *this; };
      virtual ScalarProperty &operator=( int value ) override
      { m_value = static_cast<InternalType>( value ); return *this; };
      virtual ScalarProperty &operator=( float value ) override
      { m_value = static_cast<InternalType>( value ); return *this; };
      virtual ScalarProperty &operator=( double value ) override
      { m_value = static_cast<InternalType>( value ); return *this; };

      virtual operator unsigned int() override
      {
        return static_cast<unsigned int>( m_value );
      };
      virtual operator int() override
      {
        return static_cast<int>( m_value );
      };
      virtual operator float() override
      {
        return static_cast<float>( m_value );
      };
      virtual operator double() override
      {
        return static_cast<double>( m_value );
      };

    public:
      InternalType value() const { return m_value; };

    private :
      InternalType m_value;
  };

  class ListProperty : public IProperty
  {
    public:

      IProperty &operator=( unsigned int value ) override { return *this; };
      IProperty &operator=( int value ) override  { return *this; };
      IProperty &operator=( float value ) override  { return *this; };
      IProperty &operator=( double value ) override  { return *this; };

      operator unsigned int() override { return 0; };
      operator int() override { return 0; };
      operator float() override { return 0; };
      operator double() override { return 0; };

      void define(Type type, size_t size);
      size_t size() const { return list.size(); }

      IProperty &value(size_t index);

    private:
      std::vector<std::unique_ptr<IProperty>> list;
      std::unique_ptr<IProperty> getScalarProperty( Type type );
  };

  struct ElementDefinition;

  class ElementBuffer
  {
    public:
      ElementBuffer() = default;
      ElementBuffer( const ElementDefinition &definition );

    public:
      void reset( size_t size );
      size_t size() const { return properties.size(); };
      IProperty &operator[]( size_t index );

    private:
      void appendScalarProperty( Type type );
      void appendListProperty( Type type );
      std::unique_ptr<IProperty> getScalarProperty( Type type );

    private:
      std::vector<std::unique_ptr<IProperty>> properties;
  };

  struct Property
  {
    Property( const std::string &name, Type type, bool isList )
      : name( name ), type( type ), isList( isList ) {};

    std::string name;
    Type type;
    bool isList;
  };

  typedef std::size_t ElementSize;

  struct Element
  {
    Element( const std::string &name, ElementSize size, const std::vector<Property> &properties )
      : name( name ), size( size ), properties( properties ) {};

    std::string name;
    ElementSize size;
    std::vector<Property> properties;
  };

  typedef std::function< void( ElementBuffer & ) > ElementReadCallback;

  class FileParser;

  typedef std::vector<Element> ElementsDefinition;

  class File
  {
    public:
      File( const std::string &filename );
      ~File();

      ElementsDefinition definitions() const;
      void setElementReadCallback( std::string elementName, ElementReadCallback &readCallback );
      void read();

    public:
      enum class Format
      {
        ASCII,
        BINARY_LITTLE_ENDIAN,
        BINARY_BIG_ENDIAN
      };

    private:
      std::string m_filename;
      std::unique_ptr<FileParser> m_parser;
  };


  typedef std::function< void( ElementBuffer &, size_t index ) > ElementWriteCallback;

  class FileOut
  {
    public:
      FileOut( const std::string &filename, File::Format format );

      void setElementsDefinition( const ElementsDefinition &definitions );
      void setElementWriteCallback( const std::string &elementName, ElementWriteCallback &writeCallback );
      void write();

    private:
      void createFile();
      void writeHeader();
      void writeData();

    private:
      std::string m_filename;
      File::Format m_format;
      ElementsDefinition m_definitions;
      std::map<std::string, ElementWriteCallback> m_writeCallbacks;
  };
}