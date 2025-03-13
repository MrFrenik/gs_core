local util = require("spec.util")

describe("subtyping of interfaces:", function()
   it("record inherits interface array definition", util.check([[
      local interface MyInterface
         is {MyInterface}
         x: integer
      end

      local record MyRecord
         is MyInterface
      end

      local r: MyRecord = {}
      print(#r)
   ]]))

   it("record <: interface", util.check([[
      local interface MyInterface
         x: integer
      end

      local record MyRecord is MyInterface
      end

      local function f(p: MyInterface)
         print(p.x)
      end

      local r: MyRecord = { x = 2 }
      f(r)
   ]]))

   it("prototype record <: interface", util.check([[
      local interface MyInterface
         x: integer
      end

      local record MyRecord is MyInterface
      end

      local function f(p: MyInterface)
         print(p.x)
      end

      MyRecord.x = 2
      f(MyRecord)
   ]]))

   it("record <: interface (regression test for #859, example without generics)", util.check([[
      local interface IFoo
         get_value: function(self): integer
      end

      local record Foo is IFoo
         _value: integer
      end

      function Foo:get_value():integer
         return self._value
      end

      function Foo.new(value: integer):Foo
         local fields = { _value = value }
         return setmetatable(fields, { __index = Foo })
      end

      local function create_foo(value: integer):IFoo
         local foo = Foo.new(value)
         return foo
      end

      local foo = create_foo(5)
      print(foo:get_value())
    ]]))

   it("generic record <: generic interface (regression test for #859)", util.check([[
      local interface IFoo<T>
         get_value: function(self): T
      end

      local record Foo<T> is IFoo<T>
         _value: T
      end

      function Foo:get_value():T
         return self._value
      end

      function Foo.new(value: T):Foo<T>
         local fields = { _value = value }
         return setmetatable(fields, { __index = Foo })
      end

      local function create_foo<T>(value: T):IFoo<T>
         local foo = Foo.new(value)
         return foo

         -- Have to do this instead for now:
         -- return foo as IFoo<T>
      end

      ------------------------

      local foo = create_foo(5)
      print(foo:get_value())
   ]]))

   it("regression test when matching against an unknown type", util.check_type_error([[
      local interface B
      end

      local x: B

      if x is W then
      end
   ]], {
      { msg = "x (of type B) can never be a W" },
      { msg = "unknown type W" },
   }))

   it("regression test for #830", util.check_lines([[
      local interface IFoo
      end

      local record Foo is IFoo
      end

      local function bar(_value:Foo)
      end

      local function qux(_value:IFoo)
      end

      local foo:Foo

   ]], {
      { line = "bar(foo)" },
      { line = "bar(Foo)" },
      { line = "bar(IFoo)", err = "IFoo is not a Foo" },
      { line = "bar(foo as Foo)" },
      { line = "bar(Foo as Foo)" },
      { line = "bar(IFoo as Foo)", err = "interfaces are abstract" },
      { line = "bar(foo as IFoo)", err = "IFoo is not a Foo" },
      { line = "bar(Foo as IFoo)", err = "IFoo is not a Foo"  },
      { line = "bar(IFoo as IFoo)", err = "interfaces are abstract" },
      { line = "qux(foo)" },
      { line = "qux(Foo)" },
      { line = "qux(IFoo)", err = "interfaces are abstract" },
      { line = "qux(foo as Foo)" },
      { line = "qux(Foo as Foo)" },
      { line = "qux(IFoo as Foo)", err = "interfaces are abstract" },
      { line = "qux(foo as IFoo)" },
      { line = "qux(Foo as IFoo)" },
      { line = "qux(IFoo as IFoo)", err = "interfaces are abstract" },
   }))
end)
