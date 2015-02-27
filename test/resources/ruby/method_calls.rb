
values = [
    false, true,
    1, 256,
    2, 2345,
    'A', '#',
    -12234, 233344,
    -2331, 33322323,
    23.2, -233.3,
    -2333113.4, -223567433.323,
    '', 'ASDFFAS',
    Identity.new, nil
]

a_values = [
    [false, true],
    [1, 256],
    [2, 2345],
    ['A', '#'],
    [-12234, 233344],
    [-2331, 33322323],
    [23.2, -233.3],
    [-2333113.4, -223567433.323],
    ['', 'ASDFFAS'],
    [Identity.new, nil]
]


#Since ruby does not have byte/short etc not sure how these functions map in java
values.each do |element|
    tester = Tester.new :map, element
    tester.call
end


a_values.each do |array|
    tester = Tester.new :map, array
    tester.call
end

tester = Tester.new :map, nil
tester.void_call



values.each do |element|
    tester = Tester.new :map_static, element
    tester.static_call void: false, static: true
end

a_values.each do |array|
    tester = Tester.new :map_static, array
    tester.static_call void: false, static: true
end

tester = Tester.new :map_static, nil
tester.static_void_call void:true, static: true