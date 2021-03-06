#ifndef dplyr_tools_GroupedDataFrame_H
#define dplyr_tools_GroupedDataFrame_H

namespace Rcpp {
    
    class GroupedDataFrame ;
    
    class GroupedDataFrameIndexIterator {
    public:
        GroupedDataFrameIndexIterator( const GroupedDataFrame& gdf_ ) ; 
        
        GroupedDataFrameIndexIterator& operator++() ;
        
        SlicingIndex operator*() const  ;
        
        int i ;
        const GroupedDataFrame& gdf ;
        ListOf<IntegerVector> indices ;
    } ;
    
    class GroupedDataFrame {
    public:
        typedef GroupedDataFrameIndexIterator group_iterator ;
        GroupedDataFrame( SEXP x): 
            data_(x),
            group_sizes(), 
            biggest_group_size(0),
            symbols( data_.attr("vars") ),
            labels()
        {
            // handle lazyness
            bool is_lazy = Rf_isNull( data_.attr( "group_sizes") ) || Rf_isNull( data_.attr( "labels") ) ;
            if( is_lazy ){
                data_ = build_index_cpp( data_) ;      
            }
            group_sizes = data_.attr( "group_sizes" );
            biggest_group_size  = data_.attr( "biggest_group_size" ) ;
            labels = data_.attr( "labels" );
        }
        
        group_iterator group_begin() const {
            return GroupedDataFrameIndexIterator( *this ) ;
        }
        
        SEXP symbol( int i) const {
            return symbols[i] ;    
        }
        
        DataFrame& data() { 
            return data_ ;
        }
        const DataFrame& data() const { 
            return data_ ;
        }
        
        inline int ngroups() const {
            return group_sizes.size() ;    
        }
        
        inline int nvars() const {
            return labels.size() ;    
        }
        
        inline int nrows() const {
            return data_.nrows() ;
        }
        
        inline SEXP label(int i) const {
            return labels[i];
        }
        
        inline int max_group_size() const{
            return biggest_group_size ;
        }
        
        inline bool has_group(SEXP g) const {
            SEXP symb = as_symbol(g) ;
            int n = symbols.size() ;
            for( int i=0; i<n; i++){
              if( symbols[i] == symb ) return true ;  
            }
            return false ;
        }
        
        inline const IntegerVector& get_group_sizes() const {
            return group_sizes ;    
        }
        
    private:
        
        DataFrame data_ ;
        IntegerVector group_sizes ;
        int biggest_group_size ;
        ListOf<Symbol> symbols ;
        DataFrame labels ;
        
    } ;
    
    template <>
    inline bool is<GroupedDataFrame>( SEXP x){
        return Rf_inherits(x, "grouped_df" ) && Rf_getAttrib(x, Rf_install("vars") ) != R_NilValue ;
    }
    
    inline GroupedDataFrameIndexIterator::GroupedDataFrameIndexIterator( const GroupedDataFrame& gdf_ ) : 
        i(0), gdf(gdf_), indices(gdf.data().attr("indices")) {}
    
    inline GroupedDataFrameIndexIterator& GroupedDataFrameIndexIterator::operator++(){
        i++; 
        return *this ;
    }
    
    inline SlicingIndex GroupedDataFrameIndexIterator::operator*() const {
        return SlicingIndex( indices[i], i ) ;
    }
    
    
}

#endif
