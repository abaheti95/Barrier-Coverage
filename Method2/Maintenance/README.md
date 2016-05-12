TODOS:
- What is the left and right sensors are allowed to detach from the boundaries? How to work with this
- Restrict the movement if the communication range in violated (things will work very differently in this case I suppose)

Theory behind restricting the movement till communication range
We have a current sensor(sensor which is being moved), target sensor(sensor with which we are checking for any violations) and force(applied on the current sensor)
Let
current sensor's location	: `(x1, y1)`
target sensor's location	: `(x2, y2)`
force on current sensor		: `(fx, fy)`
Communication range			: `Rc`

Assuming that moving current sensor with force will result in violation of communication range,
we move the current sensor with force `(k*fx, k*fy)`, where `k` is a positive fraction

Now we want that 
magnitude(vector from target sensor location to current sensor location + force vector) = Rc

`||((x1 - x2),(y1 - y2)) + (k*fx, k*fy)|| = Rc`
where `|| x ||` = L2 norm
therefore
`=>		||(((x1 - x2) + k*fx),((y1-y2) + k*fy))|| = Rc`
`=>		k^2*fx^2 + 2*k*(x1 - x2) + (x1 - x2)^2 + k^2*fy^2 + 2*k*(y1 - y2) + (y1 - y2)^2 = Rc^2`
`=>		(fx^2 + fy^2)*k^2 + 2*(fx*(x1 - x2) + fy*(y1 - y2))*k + ((x1 - x2)^2 + (y1 - y2)^2 - Rc^2) = 0`
This is a quadratic equation in `k` with
`a = (fx^2 + fy^2)`
`b = 2*(fx*(x1 - x2) + fy*(y1 - y2))`
`c = (x1 - x2)^2 + (y1 - y2)^2 - Rc^2`
As we can observer that c is always negative or 0
Therefore our solutions for `k` will be always real
Out of the two real solutions we will discard the negative solution of k



