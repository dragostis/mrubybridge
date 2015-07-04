def assert(left, right)
  raise "#{left} != #{right}" unless left == right
end

identity = Identity.new
values = [
  [true, "boolean"],
  [126, "byte"],
  [10234, "short"],
  [10234, "char"],
  [1234567890, "int"],
  [1234567890, "long"],
  [3.5, "float"],
  [3.5, "double"],
  ["foo", "string"]
]

identity.map

# mapping value

values.each do |value, type|
  assert identity.send("map_" + type, value), value
end

left = identity
right = identity.map_object identity

raise "#{left} != #{right}" unless left.eql right

# mapping [value]

values.each do |value, type|
  assert identity.send("map_" + type + "_array", [value]), [value]
end

left = [identity]
right = identity.map_object_array [identity]

raise "#{left} != #{right}" unless left[0].eql right[0]

Identity.map_static

# mapping static value

values.each do |value, type|
  assert Identity.send("map_static_" + type, value), value
end

left = identity
right = Identity.map_static_object identity

raise "#{left} != #{right}" unless left.eql right

# mapping static [value]

values.each do |value, type|
  assert Identity.send("map_static_" + type + "_array", [value]), [value]
end

left = [identity]
right = Identity.map_static_object_array [identity]

raise "#{left} != #{right}" unless left[0].eql right[0]
