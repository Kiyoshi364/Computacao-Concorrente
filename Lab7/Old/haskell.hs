import Data.Char (isDigit)
import Parser
import Control.Applicative

main :: IO ()
main = interact $ solve . lines

type Id = Int
type Index = Int

data Acontecimento
    = QueroUsar
    | Bloqueado
    | Notificado
    | Terminado Index Id
    deriving Show

data Mudanca
    = Produtor Id Acontecimento
    | Consumidor Id Acontecimento
    | Erro String
    deriving Show

-- (FilasProd, FilasCons, Buffer, (inicio, fim))
data Estado = Estado
    { queroProd :: [Id]
    , blockProd :: [Id]
    , queroCons :: [Id]
    , blockCons :: [Id]
    , buffer :: [Int]
    , inicio :: Int
    , fim :: Int
    , size :: Int
    }

defaultsize = 10

defaultEstado :: Estado
defaultEstado = Estado 
    { queroProd = []
    , blockProd = []
    , queroCons = []
    , blockCons = []
    , buffer    = []
    , inicio    = -1
    , fim       = -1
    , size      = defaultsize
    }

solve :: [String] -> String
solve =
      (\ (s, _) -> if s == "" then "Tudo ok!" else "Log:"++s )
    . foldl validate ("", defaultEstado)
    . map parse

-- validate
validate :: (String, Estado) -> Mudanca -> (String, Estado)
validate (err, e) (Produtor id QueroUsar)
    | elem id (queroProd e) = (err++queroErr, e)
    | elem id (blockProd e) = (err++blockErr, e)
    | otherwise             = (err          , newE)
    where newE = e{ queroProd=id:(queroProd e) }
          inicioErr = "\nProdutor("++(show id)++") quer produzir, mas "
          queroErr = inicioErr ++ "já queria antes"
          blockErr = inicioErr ++ "está bloqueado"

validate (err, e) (Produtor id Bloqueado)
    | elem id (queroProd e) = (err          , newE)
    | elem id (blockProd e) = (err++blockErr, e)
    | otherwise             = (err++nquerErr, e)
    where newE = e
              { queroProd=delete id (queroProd e), blockProd=id:(blockProd e) }
          inicioErr = "\nProdutor("++(show id)++") vai se bloquear, mas "
          nquerErr = inicioErr ++ "não queria antes"
          blockErr = inicioErr ++ "já está bloqueado"

validate (err, e) (Produtor id Notificado)
    | elem id (queroProd e) = (err++queroErr, e)
    | elem id (blockProd e) = (err          , newE)
    | otherwise             = (err++nblokErr, e)
    where newE = e
              { queroProd=id:(queroProd e), blockProd=delete id (blockProd e) }
          inicioErr = "\nProdutor("++(show id)++") foi notificado, mas "
          queroErr = inicioErr ++ "já queria antes"
          nblokErr = inicioErr ++ "não estava bloqueado"

validate (err, e) (Produtor id (Terminado index value))
    | id /= value           = (err++valueErr, newE)
    | index /= fimCerto     = (err++indexErr, newE)
    | elem id (queroProd e) = (err          , newE)
    | elem id (blockProd e) = (err++blockErr, newE)
    | otherwise             = (err++nquerErr, newE)
    where newE = e
              { queroProd=delete id (queroProd e), inicio=newInic, fim=newFim }
          newInic = if inicio e == -1 then 0 else inicio e
          newFim = if fim e == -1 then 1 else mod ((fim e) + 1) (size e)
          fimCerto = if fim e == -1 then 0 else fim e
          inicioErr = "\nProdutor("++(show id)++") terminou de escrever, mas "
          valueErr = inicioErr ++ "escreveu " ++ show value
          indexErr = inicioErr ++ "escreveu na posição " ++ show index ++ " era para ser " ++ (show $ fimCerto)
          nquerErr = inicioErr ++ "não queria antes"
          blockErr = inicioErr ++ "estava bloqueado"

validate (err, e) (Consumidor id QueroUsar)
    | elem id (queroCons e) = (err++queroErr, e)
    | elem id (blockCons e) = (err++blockErr, e)
    | otherwise             = (err          , newE)
    where newE = e{ queroCons=id:(queroCons e) }
          inicioErr = "\nConsumidor("++(show id)++") quer consumir, mas "
          queroErr = inicioErr ++ "já queria antes"
          blockErr = inicioErr ++ "está bloqueado"

validate (err, e) (Consumidor id Bloqueado)
    | elem id (queroCons e) = (err          , newE)
    | elem id (blockCons e) = (err++blockErr, e)
    | otherwise             = (err++nquerErr, e)
    where newE = e
              { queroCons=delete id (queroCons e), blockCons=id:(blockCons e) }
          inicioErr = "\nConsumidor("++(show id)++") vai se bloquear, mas "
          nquerErr = inicioErr ++ "não queria antes"
          blockErr = inicioErr ++ "já está bloqueado"

validate (err, e) (Consumidor id Notificado)
    | elem id (queroCons e) = (err++queroErr, e)
    | elem id (blockCons e) = (err          , newE)
    | otherwise             = (err++nblokErr, e)
    where newE = e
              { queroCons=id:(queroCons e), blockCons=delete id (blockCons e) }
          inicioErr = "\nConsumidor("++(show id)++") foi notificado, mas "
          queroErr = inicioErr ++ "já queria antes"
          nblokErr = inicioErr ++ "não estava bloqueado"

validate (err, e) (Consumidor id (Terminado index value))
    | index /= inicioCerto  = (err++indexErr, newE)
    | elem id (queroCons e) = (err          , newE)
    | elem id (blockCons e) = (err++blockErr, newE)
    | otherwise             = (err++nquerErr, newE)
    where newE = e
              { queroCons=delete id (queroCons e), inicio=newInic, fim=newFim }
          newInic = if mod ((inicio e) + 1) (size e) == fim e then -1
                    else mod ((inicio e) + 1) (size e)
          newFim = if mod ((inicio e) + 1) (size e) == fim e then -1 else fim e
          inicioCerto = if inicio e == -1 then 0 else inicio e
          inicioErr = "\nConsumidor("++(show id)++") terminou de pegar, mas "
          indexErr = inicioErr ++ "pegou na posição " ++ show index ++ " era para ser " ++ (show $ inicioCerto)
          nquerErr = inicioErr ++ "não queria antes"
          blockErr = inicioErr ++ "estava bloqueado"

validate (err, e) (Erro s) = (err++"\nErro: "++s, e)

delete :: Eq a => a -> [a] -> [a]
delete _  []    = []
delete a (x:xs)
     | a == x    = xs
     | otherwise = x : (delete a xs)

parse :: String -> Mudanca
parse s = (\(_,m) -> m) $
        (maybe ("", Erro "parsing Err") id $ runParser mudancaP s)

mudancaP :: Parser Mudanca
mudancaP = caso1P <|> caso2P

caso1P :: Parser Mudanca
caso1P =
    fmap strToMudanca $
    sequenceA [
          strP "Produtor" <|> strP "Consumidor"
        , strP "(" *> spanP isDigit
        , strP "): " *> (
            strP "querendo inserir" <|> strP "querendo pegar"
            <|> strP "bloqueado" <|> strP "notificado"
          )
    ]

caso2P :: Parser Mudanca
caso2P =
    fmap strToMudanca' $
    sequenceA
    [ strP "Produtor" <|> strP "Consumidor"
    , strP "(" *> spanP isDigit
    , strP ") na posição " *> spanP isDigit
    , strP ": " *>
         (strP "pegou valor: " <|> strP "inseriu valor: ")
         *> spanP isDigit
    ]

strToMudanca :: [String] -> Mudanca
strToMudanca ("Produtor":id:estado:[])
    | estado == "querendo inserir" = Produtor (read id) QueroUsar
    | estado == "bloqueado"        = Produtor (read id) Bloqueado
    | estado == "notificado"       = Produtor (read id) Notificado
    | otherwise                    = Erro $ "Produtor "++id++" "++estado

strToMudanca ("Consumidor":id:estado:[])
    | estado == "querendo pegar" = Consumidor (read id) QueroUsar
    | estado == "bloqueado"      = Consumidor (read id) Bloqueado
    | estado == "notificado"     = Consumidor (read id) Notificado
    | otherwise                  = Erro $ "Consumidor "++id++" "++estado

strToMudanca' :: [String] -> Mudanca
strToMudanca' ("Produtor":id:index:valor:[])
    = Produtor (read id) (Terminado (read index) (read valor))

strToMudanca' ("Consumidor":id:index:valor:[])
    = Consumidor (read id) (Terminado (read index) (read valor))
