# Renderer Math

## Basics

For $\vec{v_1} = \left( \begin{array}{c} x_1 \\ y_1 \end{array} \right)$, $\vec{v_2} = \left( \begin{array}{c}x_2 \\ y_2\end{array} \right)$ and $A = \left[ \begin{array}{cc} x_1 & y_1 \\ x_2 & y_2 \end{array} \right]$ <br/>
let $\vec{v1} \times \vec{v2} = \det(A) = x_1 y_2 - y_1 x_2$

Also the intersection of two line segments (calculated with Cramer's rule) is:

$A = \left[ \begin{array}{c} \vec{a_1} & \vec{a_2} \end{array} \right] ^ T ~~~ B = \left[ \begin{array}{c} \vec{b_1} & \vec{b_2} \end{array} \right] ^ T$ <br/>
$i_x = \left( \begin{array}{c} \det(A) \\ x_a1 - x_a2 \end{array} \right) \times \left( \begin{array}{c} \det(B) \\ x_b1 - x_b2 \end{array} \right)$ <br/>
$i_y = \left( \begin{array}{c} \det(A) \\ y_a1 - y_a2 \end{array} \right) \times \left( \begin{array}{c} \det(B) \\ y_b1 - y_b2 \end{array} \right)$ <br/>
$\text{intersect}(\vec{a_1}, \vec{a_2}, \vec{b_1}, \vec{b_2}) = \left( \begin{array}{c} i_x \\ i_y \end{array} \right) * \frac{1}{(\vec{a_1} - \vec{a_2}) \times (\vec{b_1} - \vec{b_2})}$ 
