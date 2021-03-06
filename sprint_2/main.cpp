//#include "search_server.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include <cfloat>
#include <numeric>

using namespace std;

/* Подставьте вашу реализацию класса SearchServer сюда */

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            words.push_back(word);
            word = "";
        } else {
            word += c;
        }
    }
    words.push_back(word);
    
    return words;
}
    
struct Document {
    int id;
    double relevance;
    int rating;
};

enum class DocumentStatus {
    ACTUAL,
    IRRELEVANT,
    BANNED,
    REMOVED,
};

class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }    
    
    void AddDocument(int document_id, const string& document, DocumentStatus status, const vector<int>& ratings) {
        const vector<string> words = SplitIntoWordsNoStop(document);
        const double inv_word_count = 1.0 / words.size();
        for (const string& word : words) {
            word_to_document_freqs_[word][document_id] += inv_word_count;
        }
        documents_.emplace(document_id, 
            DocumentData{
                ComputeAverageRating(ratings), 
                status
            });
    }
    
    vector<Document> FindTopDocuments(const string& raw_query, const DocumentStatus find_status = DocumentStatus::ACTUAL) const {
        return FindTopDocuments(raw_query, [find_status](int document_id, DocumentStatus status, int rating) { 
            return status == find_status;
        });
    }

    template <typename DocumentPredicate>
    vector<Document> FindTopDocuments(const string& raw_query, const DocumentPredicate document_predicate) const {     
        const Query query = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query, document_predicate);

        sort(matched_documents.begin(), matched_documents.end(),
                [](const Document& lhs, const Document& rhs) {
                if (abs(lhs.relevance - rhs.relevance) < 1e-6) {
                    return lhs.rating > rhs.rating;
                } else {
                    return lhs.relevance > rhs.relevance;
                }
                });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }

    int GetDocumentCount() const {
        return documents_.size();
    }
    
    tuple<vector<string>, DocumentStatus> MatchDocument(const string& raw_query, int document_id) const {
        const Query query = ParseQuery(raw_query);
        vector<string> matched_words;
        for (const string& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)) {
                matched_words.push_back(word);
            }
        }
        for (const string& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)) {
                matched_words.clear();
                break;
            }
        }
        return {matched_words, documents_.at(document_id).status};
    }
    
private:
    struct DocumentData {
        int rating;
        DocumentStatus status;
    };

    set<string> stop_words_;
    map<string, map<int, double>> word_to_document_freqs_;
    map<int, DocumentData> documents_;
    
    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }
    
    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }
    
    static int ComputeAverageRating(const vector<int>& ratings) {
        if (ratings.size() == 0) {
            return 0;
        }
        int rating_sum = 0;
        for (const int rating : ratings) {
            rating_sum += rating;
        }
        return rating_sum / static_cast<int>(ratings.size());
    }
    
    struct QueryWord {
        string data;
        bool is_minus;
        bool is_stop;
    };
    
    QueryWord ParseQueryWord(string text) const {
        bool is_minus = false;
        // Word shouldn't be empty
        if (text[0] == '-') {
            is_minus = true;
            text = text.substr(1);
        }
        return {
            text,
            is_minus,
            IsStopWord(text)
        };
    }
    
    struct Query {
        set<string> plus_words;
        set<string> minus_words;
    };
    
    Query ParseQuery(const string& text) const {
        Query query;
        for (const string& word : SplitIntoWords(text)) {
            const QueryWord query_word = ParseQueryWord(word);
            if (!query_word.is_stop) {
                if (query_word.is_minus) {
                    query.minus_words.insert(query_word.data);
                } else {
                    query.plus_words.insert(query_word.data);
                }
            }
        }
        return query;
    }
    
    // Existence required
    double ComputeWordInverseDocumentFreq(const string& word) const {
        return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
    }

    template <typename DocumentPredicate>
    vector<Document> FindAllDocuments(const Query& query, DocumentPredicate document_predicate) const {
        map<int, double> document_to_relevance;
        for (const string& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
            for (const auto [document_id, term_freq] : word_to_document_freqs_.at(word)) {
                DocumentData doc = documents_.at(document_id);
                if (!document_predicate(document_id, doc.status, doc.rating)) {
                    continue;
                }
                document_to_relevance[document_id] += term_freq * inverse_document_freq;
            }
        }
        
        for (const string& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            for (const auto [document_id, _] : word_to_document_freqs_.at(word)) {
                document_to_relevance.erase(document_id);
            }
        }

        vector<Document> matched_documents;
        for (const auto [document_id, relevance] : document_to_relevance) {
            matched_documents.push_back({
                document_id,
                relevance,
                documents_.at(document_id).rating
            });
        }
        return matched_documents;
    }
};

void PrintDocument(const Document& document) {
    cout << "{ "s
         << "document_id = "s << document.id << ", "s
         << "relevance = "s << document.relevance << ", "s
         << "rating = "s << document.rating
         << " }"s << endl;
}

/* End SearchServer */

//-------------------- ASSERTS
template <typename T>
ostream& Print(ostream& out, const T& container) {
    bool is_first_elem = true;
    for (const auto& element : container) {
        if (is_first_elem) {
            out << element;
            is_first_elem = false;
        }
        else {
            out << ", "s << element;
        }
    }
    return out;
}

template <typename T>
ostream& operator<<(ostream& out, const vector<T>& container) {
    out << "[";
    Print(out, container);
    out << "]";
    return out;
}

template <typename T>
ostream& operator<<(ostream& out, const set<T>& container) {
    out << "{";
    Print(out, container);
    out << "}";
    return out;
}

template <typename T1, typename T2>
ostream& operator<<(ostream& out, const pair<T1, T2>& container) {
    out << container.first << ": " << container.second;
    return out;
}

template <typename T1, typename T2>
ostream& operator<<(ostream& out, const map<T1, T2>& container) {
    out << "{";
    Print(out, container);
    out << "}";
    return out;
}

template <typename T, typename U>
void AssertEqualImpl(const T& t, const U& u, const string& t_str, const string& u_str, const string& file,
                     const string& func, unsigned line, const string& hint) {
    if (t != u) {
        cout << boolalpha;
        cout << file << "("s << line << "): "s << func << ": "s;
        cout << "ASSERT_EQUAL("s << t_str << ", "s << u_str << ") failed: "s;
        cout << t << " != "s << u << "."s;
        if (!hint.empty()) {
            cout << " Hint: "s << hint;
        }
        cout << endl;
        abort();
    }
}

#define ASSERT_EQUAL(a, b) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, ""s)

#define ASSERT_EQUAL_HINT(a, b, hint) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, (hint))

void AssertImpl(bool value, const string& expr_str, const string& file, const string& func, unsigned line,
                const string& hint) {
    if (!value) {
        cout << file << "("s << line << "): "s << func << ": "s;
        cout << "ASSERT("s << expr_str << ") failed."s;
        if (!hint.empty()) {
            cout << " Hint: "s << hint;
        }
        cout << endl;
        abort();
    }
}

#define ASSERT(expr) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, ""s)

#define ASSERT_HINT(expr, hint) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, (hint))

template <typename T>
void RunTestImpl(const T& t, const string& func_name) {
    try {
        t();
        cerr << func_name << " OK"s << endl;
    }
    catch (const std::exception& e) {
        cerr << "err"s << endl;
    }
}

#define RUN_TEST(func) RunTestImpl(func, #func)

//-------------------- End ASSERTS

// -------- Начало модульных тестов поисковой системы ----------

void TestAddDocument() {
    const int doc_id = 42;
    const string content1 = "cat in the city"s;
    const string content2 = "my dog like eat cat in Brazil"s;
    const vector<int> ratings = {1, 2, 3};
    
    //проверка добавления обычного документа
    {
        SearchServer server;
        server.AddDocument(doc_id, content1, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("in"s);
        ASSERT(found_docs.size() == 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id);
    }
    
    //добавление одинаковых документов
    {
        SearchServer server;
        server.AddDocument(doc_id, content1, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id, content1, DocumentStatus::ACTUAL, ratings);
        ASSERT(server.GetDocumentCount() == 1);
    }
}

void TestDocumentsSearch() {
    const int doc_id = 42;
    const string content1 = "cat in the city"s;
    const string content2 = "my dog like eat cat in Brazil"s;
    const vector<int> ratings = {1, 2, 3};

    //проверка поиска нескольких документов
    {
        SearchServer server;
        server.AddDocument(doc_id, content1, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id + 1, content2, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("cat"s);
        ASSERT_EQUAL(found_docs.size(), 2u);
    }

    //поиск определенного документа
    {
        SearchServer server;
        server.AddDocument(doc_id, content1, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id + 1, content2, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("eat"s);
        ASSERT_EQUAL(found_docs[0].id, doc_id + 1);
    }

    //поиск документов с неизвестными словами
    {
        SearchServer server;
        server.AddDocument(doc_id, content1, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id + 1, content2, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("bread"s);
        ASSERT(found_docs.empty());
    }
}

//проверка поиска с разными статусами документов
void TestSearchWithDifferentStatuses() {
    const int doc_id = 42;
    const string content1 = "cat in the city"s;
    const string content2 = "my dog like eat cat in Brazil"s;
    const vector<int> ratings = {1, 2, 3};

    //убеждаемся что первый документ находит со статусом ACTUAL
    {
        SearchServer server;
        server.AddDocument(doc_id, content1, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id + 1, content2, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("cat"s);
        ASSERT_EQUAL(found_docs.size(), 2u);
    }
    
    //Поиск по документам с наличием стутуса IRRELEVANT
    {
        SearchServer server;
        server.AddDocument(doc_id, content1, DocumentStatus::IRRELEVANT, ratings);
        server.AddDocument(doc_id + 1, content2, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("cat"s);
        ASSERT_EQUAL_HINT(found_docs.size(), 1u, "Should search only on actual documents"s);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id + 1);
    }

    //Поиск по документам с наличием стутуса BANNED
    {
        SearchServer server;
        server.AddDocument(doc_id, content1, DocumentStatus::BANNED, ratings);
        server.AddDocument(doc_id + 1, content2, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("cat"s);
        ASSERT(found_docs.size() == 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id + 1);
    }

    //Поиск по документам с наличием стутуса REMOVED
    {
        SearchServer server;
        server.AddDocument(doc_id, content1, DocumentStatus::REMOVED, ratings);
        server.AddDocument(doc_id + 1, content2, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("cat"s);
        ASSERT(found_docs.size() == 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id + 1);
    }
}

// Тест проверяет, что поисковая система исключает стоп-слова при добавлении документов
void TestExcludeStopWordsFromAddedDocumentContent() {
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = {1, 2, 3};
    // Сначала убеждаемся, что поиск слова, не входящего в список стоп-слов,
    // находит нужный документ
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("in"s);
        ASSERT(found_docs.size() == 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id);
    }

    // Затем убеждаемся, что поиск этого же слова, входящего в список стоп-слов,
    // возвращает пустой результат
    {
        SearchServer server;
        server.SetStopWords("in the"s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        ASSERT(server.FindTopDocuments("in"s).empty());
    }
    
    //проверка что он выполнит поиск даже есть есть стоп слова
    {
        SearchServer server;
        server.SetStopWords("the cat"s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("in"s);
        ASSERT(found_docs.size() == 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id);
    }
    
    //проверка пустое значение стоп слов
    {
        SearchServer server;
        server.SetStopWords(""s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("in"s);
        ASSERT(found_docs.size() == 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id);
    }
}

void TestWrongInputOfStopWords() {
    const int doc_id = 42;
    const string content1 = "cat in the city"s;
    const vector<int> ratings = {1, 2, 3};

    //тест на неправильно введеные стоп слова
    {
        SearchServer server;
        try {
            server.SetStopWords("the- i-n in- t-he"s);
        }
        catch (const std::exception& e) {
            string msg = e.what();
            ASSERT_HINT(false, "Unable to add words with minus symbol. Message=" + msg);
        }
        server.AddDocument(doc_id, content1, DocumentStatus::ACTUAL, ratings);
        const auto found_docs_in = server.FindTopDocuments("in"s);
        ASSERT(found_docs_in.size() == 1u);
        const auto found_docs_the = server.FindTopDocuments("the"s);
        ASSERT(found_docs_the.size() == 1u);
    }
}

void TestMinusWords() {
    const int doc_id = 42;
    const string content1 = "белый кот и модный ошейник"s;
    const string content2 = "пушистый кот пушистый хвост"s;
    const string content3 = "ухоженный пёс выразительные глаза"s;
    const vector<int> ratings = {1, 2, 3};

    //проверка что поиск документов вообще работает
    {
        SearchServer server;
        server.AddDocument(doc_id, content1, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id + 1, content2, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id + 2, content3, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("пушистый ухоженный кот ошейник"s);
        ASSERT(found_docs.size() == 3u);
    }
    
    //тест минус слов
    {
        SearchServer server;
        server.AddDocument(doc_id, content1, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id + 1, content2, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id + 2, content3, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("пушистый ухоженный кот -ошейник"s);
        ASSERT(found_docs.size() == 2u);
        vector<int> ids = { found_docs[0].id, found_docs[1].id };
        ASSERT(std::count(ids.begin(), ids.end(), doc_id) == 0);
    }
    {
        SearchServer server;
        server.AddDocument(doc_id, content1, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id + 1, content2, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id + 2, content3, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("-пушистый -ухоженный кот ошейник"s);
        ASSERT(found_docs.size() == 1u);
        ASSERT_EQUAL(found_docs[0].id, doc_id);
    }
}

void TestMatchDocument() {
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const string content2 = "cat in Brazil"s;
    const vector<int> ratings = {1, 2, 3};
    
    //проверка плюс слов
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const tuple<vector<string>, DocumentStatus> match_tuple = 
            server.MatchDocument("city"s, doc_id);
        const auto [matched_words, status] = match_tuple;
        ASSERT(matched_words.size() == 1u);
        ASSERT_EQUAL(matched_words[0], "city");
    }
    
    //проверка минус слов
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id + 1, content2, DocumentStatus::ACTUAL, ratings);
        const tuple<vector<string>, DocumentStatus> match_tuple = 
            server.MatchDocument("cat -city"s, doc_id);
        const auto [matched_words, status] = match_tuple;
        ASSERT(matched_words.empty());
    }
}

void TestCalcRelevance() {
	SearchServer server;
    const int doc_id = 42;
    const string content1 = "белый кот и модный ошейник"s;
    const string content2 = "ухоженный пёс"s;
    const string content3 = "ухоженный воспитанный пёс"s;
    const vector<int> ratings = {1, 2, 3};
	server.AddDocument(doc_id, content1, DocumentStatus::ACTUAL, ratings);
	server.AddDocument(doc_id + 1, content2, DocumentStatus::ACTUAL, ratings);
	server.AddDocument(doc_id + 2, content3, DocumentStatus::ACTUAL, ratings);
	
    //расчет IDF слов
    const auto founded_docs = server.FindTopDocuments("кот ухоженный"s);
    double count_documents = static_cast<double>(server.GetDocumentCount());
	double idf_cat = log(count_documents / 1.0);     //1 раз встречается слово кот
	double idf_groomed = log(count_documents / 2.0); //2 раза встречается слово ухоженный
    
    //расчет релевантности
    double content1_count_words = 5.0;
    double content2_count_words = 2.0;
    double content3_count_words = 3.0;
	vector<double> relevance = { 
        ((1.0 / content1_count_words) * idf_cat),       //1 - слово кот встречается 1 раз
		((1.0 / content2_count_words) * idf_groomed),   //1 - слово ухоженный встреччается 1 раз
		((1.0 / content3_count_words) * idf_groomed)    //1 - слово ухоженный встреччается 1 раз
    };

    //сортировка документов по релевантности
	sort(relevance.begin(), relevance.end(), [](const double t1, const double t2) {
        return t1 > t2; 
    });

    //сравнение расчета релевантности класса сервера и тестовых расчетов
	ASSERT(abs(founded_docs[0].relevance - relevance[0]) < DBL_EPSILON);
	ASSERT(abs(founded_docs[1].relevance - relevance[1]) < DBL_EPSILON);
	ASSERT(abs(founded_docs[2].relevance - relevance[2]) < DBL_EPSILON);
}

//тест на сортировку релевантности
void TestRelevanceSorting() {
    const int doc_id = 42;
    const string content1 = "cat in the city"s;
    const string content2 = "my dog like eat cat in Brazil"s;
    const vector<int> ratings = {1, 2, 3};
    
    {
        SearchServer server;
        server.AddDocument(doc_id, content1, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id + 1, content2, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("Brazil cat"s);
        ASSERT_EQUAL(found_docs.size(), 2u);
        ASSERT(found_docs[0].relevance > found_docs[1].relevance);
    }
}

void TestRating() {
    const int doc_id = 42;
    const string content1 = "cat in the city"s;
    const string content2 = "my dog like eat cat in Brazil"s;
    const vector<int> ratings_empty = { };
    const vector<int> ratings1 = { 2, 2, 2 };
    const vector<int> ratings2 = { 1, 19 };
    const vector<int> ratings3 = { -10, 20 };

    //тест на пустой рейтинг. Проверяет что можно добавить документ без рейтинга
    {
        SearchServer server;
        server.AddDocument(doc_id, content1, DocumentStatus::ACTUAL, ratings_empty);
        ASSERT(server.GetDocumentCount() == 1);
    }
    
    //тесты на рейтинг
    {
        SearchServer server;
        server.AddDocument(doc_id, content1, DocumentStatus::ACTUAL, ratings1);
        const auto found_docs = server.FindTopDocuments("cat"s);
        ASSERT(found_docs.size() == 1u);
        ASSERT(found_docs[0].rating == ((2 + 2 + 2) / 3)); // ratings1 sum
    }
    {
        SearchServer server;
        server.AddDocument(doc_id, content1, DocumentStatus::ACTUAL, ratings2);
        const auto found_docs = server.FindTopDocuments("cat"s);
        ASSERT(found_docs.size() == 1u);
        ASSERT(found_docs[0].rating == ((1 + 19) / 2)); // ratings2 sum
    }
    {
        SearchServer server;
        server.AddDocument(doc_id, content1, DocumentStatus::ACTUAL, ratings3);
        const auto found_docs = server.FindTopDocuments("cat"s);
        ASSERT(found_docs.size() == 1u);
        ASSERT(found_docs[0].rating == ((-10 + 20) / 2)); //ratings3 sum
    }
}

//тесты на поиск документа с фильтром по предиканту
void TestPredicant() {
    const int doc_id = 42;
    const string content1 = "cat in the city"s;
    const string content2 = "my dog like eat cat in Brazil"s;
    const vector<int> ratings = {1, 2, 3};
    const vector<int> ratings2 = {40, 4, 50, 80};
    
    {
        SearchServer server;
        server.AddDocument(doc_id, content1, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("cat"s, DocumentStatus::BANNED);
        ASSERT(found_docs.size() == 0u);
    }
    {
        SearchServer server;
        server.AddDocument(doc_id, content1, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id + 1, content2, DocumentStatus::IRRELEVANT, ratings);
        server.AddDocument(doc_id + 2, content1, DocumentStatus::REMOVED, ratings);
        const auto found_docs = server.FindTopDocuments("cat"s, DocumentStatus::IRRELEVANT);
        ASSERT(found_docs.size() == 1u);
        ASSERT_EQUAL(found_docs[0].id, doc_id + 1);
    }
    {
        SearchServer server;
        server.AddDocument(doc_id, content1, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id + 1, content2, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("cat"s, [](int document_id, DocumentStatus status, int rating) { 
            return document_id == doc_id && status == DocumentStatus::ACTUAL && rating > 0;
        });
        ASSERT(found_docs.size() == 1u);
    }
    {
        SearchServer server;
        server.AddDocument(doc_id, content1, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id + 1, content2, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("cat"s, [](int document_id, DocumentStatus status, int rating) { 
            return status == DocumentStatus::ACTUAL && document_id != 0 && rating != 0; 
        });
        ASSERT(found_docs.size() == 2u);
    }
    {
        SearchServer server;
        server.AddDocument(doc_id, content1, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(doc_id + 1, content2, DocumentStatus::ACTUAL, ratings2);
        const auto found_docs = server.FindTopDocuments("cat"s, [](int document_id, DocumentStatus status, int rating) { 
            return rating > 10 && document_id >= doc_id && status == DocumentStatus::ACTUAL;
        });
        ASSERT(found_docs.size() == 1u);
    }
}

// Функция TestSearchServer является точкой входа для запуска тестов
void TestSearchServer() {
    RUN_TEST(TestAddDocument);
    RUN_TEST(TestDocumentsSearch);
    RUN_TEST(TestSearchWithDifferentStatuses);
    RUN_TEST(TestExcludeStopWordsFromAddedDocumentContent);
    RUN_TEST(TestWrongInputOfStopWords);
    RUN_TEST(TestMinusWords);
    RUN_TEST(TestMatchDocument);
    RUN_TEST(TestCalcRelevance);
    RUN_TEST(TestRelevanceSorting);
    RUN_TEST(TestRating);
    RUN_TEST(TestPredicant);
}

// --------- Окончание модульных тестов поисковой системы -----------

int main() {
    TestSearchServer();
    // Если вы видите эту строку, значит все тесты прошли успешно
    cout << "Search server testing finished"s << endl;
}