#include "contourcollider.h"
#include "../asset_manager.h"
#include "../tex.h"
#include "../node.h"
#include "../models/shapes.h"
#include <iostream>
#include "../util.h"


ContourCollider::ContourCollider(const std::string& file, float epsilon, int, int, int) : Collider() {

    m_data = Tex::getRaw(file, m_width, m_height);
    m_size = m_width * m_height;
    for (const auto& i : m_data) {
        std::cout << (int) i << ", ";
    }
    auto startPoint = findStartPoint();
    std::cout << "start point: " << startPoint.x << ", " << startPoint.y << "\n";
    m_contour = walkPerimeter(startPoint.x, startPoint.y);
    std::cout << "---printing contour---\n";
    std::vector<glm::vec2> pointList;
    for (size_t i = 0; i < m_contour.size(); i+=2) {
        std::cout << m_contour[i] << ", " << m_contour[i+1] << std::endl;
        pointList.push_back(glm::vec2(m_contour[i], m_contour[i+1]));
    }
    int midPoint = pointList.size() / 2;
    std::cout << "midpoint is " << midPoint << " and it's " << pointList[midPoint].x << ", " << pointList[midPoint].y << "\n";
    mm = DouglasPeucker(pointList, epsilon, 0, midPoint);
    auto secondHalf = DouglasPeucker(pointList, epsilon, midPoint+1, pointList.size()-1);
    mm.insert( mm.end(), secondHalf.begin(), secondHalf.end() );
    //for (const auto& p : ciao) {
    //    std::cout << ": " << p.x << ", " << p.y << ":\n";
    //}

}

void ContourCollider::generateDebugMesh() {
    auto model = std::make_shared<PolygonModel>(mm);
    auto node = std::make_shared<Node>();
    node->setModel(model);
    m_node->add(node);
    m_debugNode = node.get();
}

// Determines if a single pixel is solid (we test against
// alpha values, you can write your own test if you want
// to test for a different color.)
bool ContourCollider::isPixelSolid(int x, int y)
{
    // Make sure we don't pick a point outside our
    // image boundary!
    if (x < 0 || y < 0 ||
        x >= m_width || y >= m_height)
        return false;

    // Check the color value of the pixel
    // If it isn't 100% transparent, it is solid
    if (m_data[x + y * m_width] > 0)
        return true;

    // Otherwise, it's not solid
    return false;
}

// Locates the perimeter of a sprite within the texture
// starting from the upper left and moving right.
glm::ivec2 ContourCollider::findStartPoint()
{
    // Scan along the whole image
    for (int pixel = 0; pixel < m_size; pixel++)
    {
        // If the pixel is not entirely transparent
        // we've found a start point
        if (m_data[pixel] > 0)
            return glm::ivec2(pixel % m_width, pixel / m_width);
    }

    // If we get here
    // we've scanned the whole image and found nothing.
    return glm::ivec2(0);
}

// Determines and sets the state of the 4 pixels that
// represent our current state, and sets our current and
// previous directions
void ContourCollider::step(int x, int y)
{
    // Scan our 4 pixel area
    bool upLeft = isPixelSolid(x-1, y-1);
    bool upRight = isPixelSolid(x, y-1);
    bool downLeft = isPixelSolid(x-1, y);
    bool downRight = isPixelSolid(x, y);

    // Store our previous step
    previousStep = nextStep;

    // Determine which state we are in
    unsigned state = 0;

    if (upLeft)
        state |= 1u;
    if (upRight)
        state |= 2u;
    if (downLeft)
        state |= 4u;
    if (downRight)
        state |= 8u;

    // State now contains a number between 0 and 15
    // representing our state.
    // In binary, it looks like 0000-1111 (in binary)

    // An example. Let's say the top two pixels are filled,
    // and the bottom two are empty.
    // Stepping through the if statements above with a state
    // of 0b0000 initially produces:
    // Upper Left == true ==>  0b0001
    // Upper Right == true ==> 0b0011
    // The others are false, so 0b0011 is our state
    // (That's 3 in decimal.)

    // Looking at the chart above, we see that state
    // corresponds to a move right, so in our switch statement
    // below, we add a case for 3, and assign Right as the
    // direction of the next step. We repeat this process
    // for all 16 states.

    // So we can use a switch statement to determine our
    // next direction based on
    switch (state )
    {
        case 1: nextStep = StepDirection::UP; break;
        case 2: nextStep = StepDirection::RIGHT; break;
        case 3: nextStep = StepDirection::RIGHT; break;
        case 4: nextStep = StepDirection::LEFT; break;
        case 5: nextStep = StepDirection::UP; break;
        case 6:
            if (previousStep== StepDirection::UP)
            {
                nextStep = StepDirection::LEFT;
            }
            else
            {
                nextStep = StepDirection::RIGHT;
            }
            break;
        case 7: nextStep = StepDirection::RIGHT; break;
        case 8: nextStep = StepDirection::DOWN; break;
        case 9:
            if (previousStep== StepDirection::RIGHT)
            {
                nextStep = StepDirection::UP;
            }
            else
            {
                nextStep = StepDirection::DOWN;
            }
            break;
        case 10: nextStep = StepDirection::DOWN; break;
        case 11: nextStep = StepDirection::DOWN; break;
        case 12: nextStep = StepDirection::LEFT; break;
        case 13: nextStep = StepDirection::UP; break;
        case 14: nextStep = StepDirection::LEFT; break;
        default:
            nextStep = StepDirection::NONE;
            break;
    }
}

std::vector<float> ContourCollider::walkPerimeter(int startX, int startY)
{
    // Do some sanity checking, so we aren't
    // walking outside the image
    if (startX< 0)
        startX= 0;
    if (startX> m_width)
        startX= m_width;
    if (startY< 0)
        startY= 0;
    if (startY> m_height)
        startY= m_height;

    // Set up our return list
    std::vector<float> pointList;

    // Our current x and y positions, initialized
    // to the init values passed in
    int x = startX;
    int y = startY;

    // The main while loop, continues stepping until
    // we return to our initial points
    do
    {
        // Evaluate our state, and set up our next direction
        step(x, y);

        // If our current point is within our image
        // add it to the list of points
        if (x >= 0 &&
            x < m_width &&
            y >= 0 &&
            y < m_height) {
            pointList.push_back(x);
            pointList.push_back(m_height-y);
        }

        switch (nextStep)
        {
            case StepDirection::UP:    y--; break;
            case StepDirection::LEFT:  x--; break;
            case StepDirection::DOWN:  y++; break;
            case StepDirection::RIGHT: x++; break;
            default:
                break;
        }
    } while (x != startX|| y != startY);

    return pointList;
}