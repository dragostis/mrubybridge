module Boolean; end

class TrueClass; include Boolean; end
class FalseClass; include Boolean; end

class JavaMethod
  def initialize(pointer, name, signature, java_class)
    @pointer = pointer
    @name = name
    @signature = signature.clone
    @java_class = java_class

    @signature.chop! if name == :construct

    @types = get_types signature
  end

  def get_types(signature)
    types = []

    signature = signature[1..-1]

    array = false

    while signature.index ')'
      type = signature[0]

      next_index = 1

      case type
      when 'Z'
        types << Boolean unless array
      when 'C', 'B', 'S', 'I', 'J'
        types << Fixnum unless array
      when 'F', 'D'
        types << Float unless array
      when '['
        types << Array

        array = true
      when 'L'
        last_index = signature.index ';'

        types << get_class(signature[1..(last_index - 1)]) unless array

        next_index = last_index + 1
      end

      array = false unless type == '['

      signature = signature[next_index..-1]
    end

    types
  end

  def get_class(java_class)
    class_name = java_class.split('/').last

    return String if class_name == 'java/lang/String'
    return JavaClass if class_name == 'java/lang/Object'

    Kernel.const_get class_name
  end

  def matches?(args)
    return true if @types.empty? && args.empty?
    return false unless @types.length == args.length

    @types && (0...(@types.length)).map { |i| args[i].kind_of? @types[i] }.inject(:&)
  end

  def call(args, object=nil)
    if object
      JavaMethod.java_call(@pointer, object.pointer, @signature, @java_class, *(get_value args))
    else
      JavaMethod.java_call_static(@pointer, @signature, @java_class, *(get_value args))
    end
  end

  def get_value(arg)
    return arg.map { |a| get_value a } if arg.kind_of? Array

    arg.kind_of?(JavaClass) ? arg.pointer : arg
  end

  private :get_types, :get_class
end

class JavaClass
  attr_accessor :pointer
  attr_reader :name

  def initialize(*args)
    @pointer = construct(*args)
  end

  def method_missing(name, *args)
    return super unless method = self.class.match_instance_method(name, args)

    method.call(args, self)
  end

  class << self
    attr_accessor :java_class

    def instance_methods
      @instance_methods ||= {}
    end

    def class_methods
      @class_methods ||= {}
    end

    def set_instance_method(pointer, name, signature)
      name = name.to_sym

      unless instance_methods[name].nil?
        instance_methods[name] << JavaMethod.new(pointer, name, signature, java_class)
      else
        instance_methods[name] = [JavaMethod.new(pointer, name, signature, java_class)]
      end
    end

    def set_class_method(pointer, name, signature)
      name = name.to_sym

      unless class_methods[name].nil?
        class_methods[name] << JavaMethod.new(pointer, name, signature, java_class)
      else
        class_methods[name] = [JavaMethod.new(pointer, name, signature, java_class)]
      end
    end

    def method_missing(name, *args)
      return super unless method = match_class_method(name, args)

      method.call(args)
    end

    def match_instance_method(name, args)
      instance_methods[name] && instance_methods[name].select { |method| method.matches?(args) }.first
    end

    def match_class_method(name, args)
      class_methods[name] && class_methods[name].select { |method| method.matches?(args) }.first
    end
  end
end
