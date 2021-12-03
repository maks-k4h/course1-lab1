//---------------------------------------------------------------------------//
#include <iostream>
#include <vector>
#include <fstream>
#include <random>

//---------------------------------------------------------------------------//
#define MAX_AREA 1000
#define MAX_BEDROOMS 20
#define MAX_BATHROOMS 10
#define MAX_TIME_TO_METRO 60 // minutes
#define MAX_COST 1e6

//---------------------------------------------------------------------------//
#define LEARNING_SPEED 0.00001 // explored by experimentation
#define LEARNING_ITERATIONS 500 // just enough

//---------------------------------------------------------------------------//
enum class error_processing{
    flexible,   // communicates with the user if input data is incorrect

    strict      // throws an exception if input data is incorrect
                // was used for testing by reading data from a file
};

//---------------------------------------------------------------------------//

// methods for reading particular data
void   ignore_line(std::istream&);
double read_area(std::istream&, error_processing);
double read_bedrooms_num(std::istream&, error_processing);
double read_bathrooms_num(std::istream&, error_processing);
double read_time_to_metro(std::istream&, error_processing);
double read_area_type(std::istream&, error_processing);// 1-commercial, 0-not
double read_cost(std::istream&, error_processing);

// reads info into matrix of parameters and vector of costs
void   read_from_stream(std::istream&,
                        std::vector<std::vector<double>>&,
                        std::vector<double>&,
                        error_processing);

void   read_features_to_predict(std::istream&,
                                std::vector<double>&,
                                error_processing);

// the studying part's methods
double predict(const std::pair<std::vector<double>, double>&,
               std::vector<double>);

std::pair<std::vector<double>, double> init_weights(unsigned);
void   train(std::pair<std::vector<double>, double>&,    // weights
             const std::vector<std::vector<double>>&,    // input example
             const std::vector<double>&,                 // output example
             double,                                     // learning speed
             unsigned);                                  // iterations

std::vector<double> dot(                                 // multiplication
        const std::vector<std::vector<double>>&,         // of the matrix
        const std::vector<double>&);                     // and the vector-column

std::vector<double> add_num_to_vec(std::vector<double>, double);
std::vector<double> add_vectors(std::vector<double>,
                                std::vector<double>);
std::vector<double> mul_vec_num(std::vector<double>, double);
std::vector<double> vec_of_ones(unsigned);
std::vector<std::vector<double>> transpose(std::vector<std::vector<double>>);
double              sum_vector(std::vector<double>);

//---------------------------------------------------------------------------//

int main()
{
    std::fstream test_file("/Users/makskonevych/Documents/Cpp/test_prog5/test_.txt");

    std::vector<std::vector<double>> params_matrix;
    std::vector<double> prices_vector;
    try{
        read_from_stream(test_file,params_matrix,
                         prices_vector,error_processing::strict);

        std::pair<std::vector<double>, double> weights;
        weights = init_weights(params_matrix[0].size());
        train(weights,
              params_matrix,
              prices_vector,
              LEARNING_SPEED,
              LEARNING_ITERATIONS);
        while(true){
            std::cout << "\n\n________________\n";
            std::vector<double> features_to_predict;
            read_features_to_predict(std::cin,
                                     features_to_predict,
                                     error_processing::flexible);
            std::cout << "\nIt will cost around "
                << predict(weights,features_to_predict)
                << ".";
        }
    }
    catch(std::string err) {
        std::cout <<"\nError:\n\t"<< err;
    }
    return 0;
}


//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//

void ignore_line(std::istream& is){
    std::string ignoredL;
    std::getline(is, ignoredL);
}

double read_area(std::istream& is, error_processing er_proc){
    double result;
    while(true){
        char c;
        std::cout << "Enter the area(from 0 to "
            <<MAX_AREA<<"): ";
        is >> c;
        if(std::isdigit(c)){
            is.putback(c);
            is >> result;
            if(result > MAX_AREA){
                std::cout << "Area is too big!\n";
                ignore_line(is);
                continue;
            }
            break;
        } else if (er_proc == error_processing::flexible) {
            std::cout << "Incorrect area, try ones more!\n";
            ignore_line(is);
            continue;
        } else
            throw (std::string("Area is incorrect!"));
    }
    return result;
}

double read_bedrooms_num(std::istream& is, error_processing er_proc){
    double result;
    while(true){
        char c;
        std::cout << "Enter the num of bedrooms(from 0 to "
            << MAX_BEDROOMS<<"): ";
        is >> c;
        if(std::isdigit(c)){
            is.putback(c);
            is >> result;
            if(result > MAX_BEDROOMS){
                std::cout << "The number of bedrooms is too big!\n";
                ignore_line(is);
                continue;
            }
            break;
        } else if (er_proc == error_processing::flexible) {
            std::cout << "Incorrect number of bedrooms, try ones more!\n";
            ignore_line(is);
            continue;
        } else
            throw (std::string("Number of bedrooms is incorrect!"));
    }
    return result;
}

double read_bathrooms_num(std::istream& is, error_processing er_proc){
    double result;
    while(true){
        char c;
        std::cout << "Enter the num of bathrooms(from 0 to "
                  << MAX_BATHROOMS<<"): ";
        is >> c;
        if(std::isdigit(c)){
            is.putback(c);
            is >> result;
            if(result > MAX_BATHROOMS){
                std::cout << "The number of bathrooms is too big!\n";
                ignore_line(is);
                continue;
            }
            break;
        } else if (er_proc == error_processing::flexible) {
            std::cout << "Incorrect number of bathrooms, try ones more!\n";
            ignore_line(is);
            continue;
        } else
            throw (std::string("Number of bathrooms is incorrect!"));
    }
    return result;
}
double read_time_to_metro(std::istream& is, error_processing er_proc){
    double result;
    while(true){
        char c;
        std::cout << "Enter the time to metro(from 0 to "
                  << MAX_TIME_TO_METRO<<"): ";
        is >> c;
        if(std::isdigit(c)){
            is.putback(c);
            is >> result;
            if(result > MAX_TIME_TO_METRO){
                std::cout << "The time to metro is too big!\n";
                ignore_line(is);
                continue;
            }
            break;
        } else if (er_proc == error_processing::flexible) {
            std::cout << "Incorrect time, try ones more!\n";
            ignore_line(is);
            continue;
        } else
            throw (std::string("Time to metro is entered incorrectly!"));
    }
    return result;
}

double read_area_type(std::istream& is, error_processing er_proc){
    std::cout << "Enter 'yes' if it's commercial area, 'no' if not: ";
    char c;
    is >> c;
    ignore_line(is);
    if(std::tolower(c) == 'y') return 1;
    return 0;
}

double read_cost(std::istream& is, error_processing er_proc){
    double result;
    while(true){
        char c;
        std::cout << "Enter the cost(from 0 to "
                  << MAX_COST<<"): ";
        is >> c;
        if(std::isdigit(c)){
            is.putback(c);
            is >> result;
            if(result > MAX_COST){
                std::cout << "The cost is too big!\n";
                ignore_line(is);
                continue;
            }
            break;
        } else if (er_proc == error_processing::flexible) {
            std::cout << "Incorrect cost, try ones more!\n";
            ignore_line(is);
            continue;
        } else
            throw (std::string("The cost is entered incorrectly!"));
    }
    return result;
}

void read_from_stream(std::istream& is,
                      std::vector<std::vector<double>>& params_matrix,
                      std::vector<double>& price_vector,
                      error_processing er_proc)
{
    std::cout<<"Enter the information about the realty.\n"
               "To end entering 'learning data' put 0 in area.\n";
    while(true){
        double area = read_area(is, er_proc);
        if(area==0||is.eof()) break;
        params_matrix.emplace_back(std::vector<double>(
                {area,
                    read_bedrooms_num(is, er_proc),
                    read_bathrooms_num(is, er_proc),
                    read_time_to_metro(is, er_proc),
                    read_area_type(is, er_proc)}));
        price_vector.emplace_back(read_cost(is, er_proc));
    }
}

void read_features_to_predict(std::istream& is,
                              std::vector<double>& features_vector,
                              error_processing er_proc){
    std::cout<<"Enter the information about the realty "
               "so that its price can be predicted.\n";
    features_vector.emplace_back(read_area(is,er_proc));
    features_vector.emplace_back(read_bedrooms_num(is,er_proc));
    features_vector.emplace_back(read_bathrooms_num(is,er_proc));
    features_vector.emplace_back(read_time_to_metro(is, er_proc));
    features_vector.emplace_back(read_area_type(is,er_proc));
}

//---------------------------------------------------------------------------//

double predict(const std::pair<std::vector<double>, double>& weights,
               const std::vector<double> features){
    double result = 0;
    for(int i=0; i<features.size(); ++i) result+=features[i]*weights.first[i];
    return result+weights.second;
}

std::pair<std::vector<double>, double> init_weights(unsigned w_size){
    std::pair<std::vector<double>, double> weights;
    for(int i=0; i<w_size; ++i)
        weights.first.emplace_back(std::rand()*1.0/RAND_MAX);
    weights.second = std::rand()*1.0/RAND_MAX;
    return weights;
}

void train(std::pair<std::vector<double>, double>& weights,
           const std::vector<std::vector<double>>& input_example,
           const std::vector<double>& output_example,
           double speed,
           unsigned iterations){
    for(int i=0; i<iterations; ++i)
    {
        unsigned num_of_examples = output_example.size();
        //  Linear regression model:
        //  S(Input, W)->C->T(C,B)->P - | ->M(P,Output)->m

        // forward propagation
        std::vector<double> C = dot(input_example,weights.first);
        std::vector<double> P = add_num_to_vec(C, weights.second);

        // m = (P-output_example)^2 — mistake function,
        // but we only need its derivative

        // calculation of the gradients
        std::vector<double> dMdP = mul_vec_num(
                add_vectors(P, mul_vec_num(output_example,-1)),
                2);
        double                              dTdB = 1;
        std::vector<double>                 dTdC = vec_of_ones(C.size());
        std::vector<std::vector<double>>    dSdW = transpose(input_example);

        // chain rule
        double dMdB = sum_vector(mul_vec_num(dMdP,dTdB))/num_of_examples;
        std::vector<double> dMdC = dMdP; // * dTdC, but actually it doesn't change anything
        std::vector<double> dMdW = mul_vec_num(dot(dSdW,dMdC), 1.0/pow(num_of_examples,2));

        // applying the gradients
        weights.first = add_vectors(weights.first, mul_vec_num(dMdW,-speed));
        weights.second-=dMdB*speed;
    }
}

//---------------------------------------------------------------------------//

std::vector<double> dot(const std::vector<std::vector<double>>& M,
                        const std::vector<double>& Col){
    assert(M.size()>0 && M[0].size() == Col.size());
    std::vector<double> result;
    for(int row = 0; row<M.size(); ++row){
        double sum = 0;
        for(int elem = 0; elem<M[row].size(); ++elem)
            sum+=M[row][elem]*Col[elem];
        result.emplace_back(sum);
    }
    return result;
}

std::vector<double> add_num_to_vec(std::vector<double> vector, double scalar)
{
    for(int i=0; i<vector.size();++i) vector[i]+=scalar;
    return vector;
}

std::vector<double> add_vectors(std::vector<double> a,std::vector<double> b){
    for(int i=0; i<a.size(); ++i) a[i]+=b[i];
    return a;
}
std::vector<double> mul_vec_num(std::vector<double> vec, double scalar){
    for(int i=0; i<vec.size();++i) vec[i]*=scalar;
    return vec;
}

std::vector<double> vec_of_ones(unsigned length)
{
    std::vector<double> result;
    for(int i=0; i<length; ++i) result.emplace_back(1);
    return result;
}

std::vector<std::vector<double>> transpose(std::vector<std::vector<double>> M){
    assert(M.size()>0);
    std::vector<std::vector<double>> result;
    for(int col = 0; col<M[0].size(); ++col){
        std::vector<double> result_row;
        for(int row = 0; row<M.size(); ++row){
            result_row.emplace_back(M[row][col]);
        }
        result.emplace_back(result_row);
    }
    return result;
}

double sum_vector(std::vector<double> vec){
    double sum = 0;
    for(double elem : vec) sum+= elem;
    return sum;
}