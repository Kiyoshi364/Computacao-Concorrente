module Parser where

import Data.Char (isDigit)
import Control.Applicative

newtype Parser a = Parser { runParser :: String -> Maybe (String, a) }

instance Functor Parser where
    fmap f (Parser p) = Parser $
        \ input ->
            case p input of
                Just (s, a) -> Just (s, f a)
                Nothing     -> Nothing

instance Applicative Parser where
    pure a = Parser $ \ input -> Just (input, a)

    (Parser p1) <*> (Parser p2) = Parser $
        \ input ->
            case p1 input of
                Nothing       -> Nothing
                Just (input', f) -> case p2 input' of
                                     Just (s, a) -> Just (s, f a)
                                     Nothing     -> Nothing

instance Alternative Parser where
    empty = Parser $ \_ -> Nothing

    (Parser p1) <|> (Parser p2) = Parser $ \input -> p1 input <|> p2 input

charP :: Char -> Parser Char
charP c = Parser f
    where f = \ (x:xs) ->
              case c == x of
                 True  -> Just (xs, c)
                 _     -> Nothing

spaceP :: Parser Char
spaceP = charP ' '

strP :: String -> Parser String
strP = sequenceA . map charP

spanP :: (Char -> Bool) -> Parser String
spanP f = Parser $ \ input ->
    let (parsed, s) = span f input
        in Just (s, parsed)

intP :: Parser Int
intP = Parser $ \ input ->
        case runParser (spanP isDigit) input of
            Just (_, [])  -> Nothing
            Just (s, str) -> Just (s, read str)
            _             -> Nothing

compose2 :: (a -> b -> c) -> Parser a -> Parser b -> Parser c
compose2 = liftA2

compose2' :: (a -> b -> c) -> Parser a -> Parser b -> Parser c
compose2' f (Parser p1) (Parser p2) = Parser $ \ input ->
    case p1 input of
        Just (input', x) ->
            case p2 input' of
                 Just (s, y) -> Just (s, f x y)
                 _           -> Nothing
        _ -> Nothing

compose2'' :: (a -> b -> c) -> Parser a -> Parser b -> Parser c
compose2'' f p1 p2 = (pure f) <*> p1 <*> p2
