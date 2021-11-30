const mongoose = require('mongoose');

const movieSchema = new mongoose.Schema({
    Position: Number,
    Created: String,
    Modified: String,
    Description: String,
    Title: String,
    URL: String,
    'Title Type': String,
    'IMDb Rating': Number,
    'Runtime (mins)': Number,
    Year: Number,
    Genres: String,
    'Num Votes': Number,
    'Release Date':String,
    'Directors':String,
    'Your Rating': Number,
    'Date Rated':String
}, {collection: 'MoviesIHaveSeen'});


var Movie = mongoose.model('Movie', movieSchema)
module.exports = Movie