
class Tester
    def initialize(method, value)
        @identity = Identity.new
        @method = method
        @value = value
    end

    def call(options = {})
        expected_return = @value

        if(options.fetch(:static, false))
            klass = @identity.class
        else
            klass = @identity
        end


        method = klass.method(@method.to_sym)
        java_value = method.call @value

        if(options.fetch(:void, false))
            expected_return = nil
        end

        java_value.eql? expected_return unless raise "Method: #{method} failed => #{value} expected but got #{java_value}"
    end
end